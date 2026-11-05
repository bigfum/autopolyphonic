#include "geometry/profile.h"
#include <glm/gtc/constants.hpp>

std::vector<glm::vec2> roundedRect(float width, float height, float radius, int cornerSegments)
{
   const float hx = 0.5f * width - radius;
   const float hy = 0.5f * height - radius;
   //  corner centers, counter-clockwise from bottom right
   const glm::vec2 centers[4] = {{hx, -hy}, {hx, hy}, {-hx, hy}, {-hx, -hy}};

   std::vector<glm::vec2> points;
   for (int c = 0; c < 4; c++)
   {
      //  each corner sweeps a quarter turn, starting where the previous one ended
      const float start = glm::half_pi<float>() * (c - 1);
      for (int i = 0; i <= cornerSegments; i++)
      {
         const float a = start + glm::half_pi<float>() * i / cornerSegments;
         points.push_back(centers[c] + radius * glm::vec2(std::cos(a), std::sin(a)));
      }
   }
   return points;
}
