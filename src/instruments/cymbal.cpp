#include "instruments/cymbal.h"
#include "geometry/lathe.h"
#include <cmath>

namespace
{
   constexpr float thickness = 0.0015f;
   constexpr float holeRadius = 0.005f;

   float surface(float r, float radius)
   {
      const float bellRadius = 0.3f * radius;
      const float bellHeight = 0.1f * bellRadius + 0.0125f;
      const float drop = 0.125f * radius;
      if (r < bellRadius)
         return bellHeight * (1 - std::pow(r / bellRadius, 3.0f));
      return -drop * std::pow((r - bellRadius) / (radius - bellRadius), 1.25f);
   }
}

Model buildCymbal(float diameter, int note)
{
   const float radius = 0.5f * diameter;
   //  underside of the bell at the hole sits at y = 0, where it rests on a stand
   const float base = surface(holeRadius, radius);
   const int steps = 40;
   std::vector<glm::vec2> profile;
   for (int i = 0; i <= steps; i++)
   {
      const float r = glm::mix(holeRadius, radius, static_cast<float>(i) / steps);
      profile.push_back({r, surface(r, radius) - base});
   }
   for (int i = steps; i >= 0; i--)
   {
      const float r = glm::mix(holeRadius, radius, static_cast<float>(i) / steps);
      profile.push_back({r, surface(r, radius) - base + thickness});
   }
   profile.push_back(profile.front());

   Model cymbal;
   cymbal.add(lathe(profile, 64), materials::bronze, glm::vec3(0), note);
   return cymbal;
}
