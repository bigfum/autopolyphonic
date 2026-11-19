#include "instruments/stand.h"
#include "geometry/extrude.h"
#include "geometry/lathe.h"
#include "geometry/profile.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
   constexpr int segments = 24;
   const glm::vec3 up(0, 1, 0);

   const std::vector<glm::vec2> legProfile = roundedRect(0.015f, 0.015f, 0.005f);
   const std::vector<glm::vec2> braceProfile = roundedRect(0.01f, 0.01f, 0.004f);
   const std::vector<glm::vec2> bracketProfile = roundedRect(0.03f, 0.04f, 0.005f);

   glm::vec3 radial(float a)
   {
      return {std::cos(a), 0, std::sin(a)};
   }

   constexpr float basketDrop = 0.03f;
   constexpr float jointDrop = 0.05f;

   glm::mat4 tiltAbout(float y, float degrees)
   {
      const glm::mat4 toPivot = glm::translate(glm::mat4(1), y * up);
      return glm::translate(glm::rotate(toPivot, glm::radians(degrees), glm::vec3(1, 0, 0)), -y * up);
   }

   MeshData tube(float r, float y0, float y1)
   {
      return lathe({{0, y0}, {r, y0}, {r, y1}, {0, y1}}, segments);
   }

   MeshData footMesh()
   {
      return lathe({{0, 0}, {0.015f, 0}, {0.015f, 0.02f}, {0, 0.035f}}, 16);
   }

   void tripod(MeshData &chrome, MeshData &rubber, int legs, float spread, float hubY, float braceY)
   {
      chrome.append(tube(0.02f, hubY - 0.03f, hubY + 0.03f));
      chrome.append(tube(0.02f, braceY - 0.02f, braceY + 0.02f));
      for (int i = 0; i < legs; i++)
      {
         const glm::vec3 out = radial(glm::two_pi<float>() * i / legs + glm::half_pi<float>());
         const glm::vec3 top = 0.02f * out + hubY * up;
         const glm::vec3 foot = spread * out + 0.02f * up;
         chrome.append(extrudeBetween(legProfile, top, foot, out));
         chrome.append(extrudeBetween(braceProfile, 0.02f * out + braceY * up, 0.5f * (top + foot), out));
         rubber.append(footMesh(), glm::translate(glm::mat4(1), glm::vec3(foot.x, 0, foot.z)));
      }
   }

   Model chromeAndRubber(MeshData chrome, MeshData rubber, const glm::mat4 &m = glm::mat4(1))
   {
      Model model;
      MeshData c, r;
      c.append(chrome, m);
      r.append(rubber, m);
      model.add(std::move(c), materials::chrome);
      model.add(std::move(r), materials::rubber);
      return model;
   }
}

Model buildCymbalStand(const StandSpec &spec)
{
   const float clampY = 0.8f;
   const float knuckleY = spec.height - 0.05f;
   const glm::mat4 tilted = glm::rotate(glm::translate(glm::mat4(1), spec.height * up), glm::radians(spec.tilt),
                                        glm::vec3(1, 0, 0));

   MeshData chrome, rubber;
   tripod(chrome, rubber, spec.legs, spec.spread, 0.4f, 0.2f);
   chrome.append(tube(0.0125f, 0.15f, clampY));
   chrome.append(tube(0.02f, clampY - 0.02f, clampY + 0.02f));
   chrome.append(tube(0.01f, clampY, knuckleY));
   chrome.append(tube(0.015f, knuckleY - 0.015f, knuckleY + 0.015f));
   chrome.append(tube(0.005f, knuckleY, spec.height - 0.01f));
   chrome.append(tube(0.005f, -0.01f, 0.04f), tilted);
   rubber.append(tube(0.02f, -0.01f, 0), tilted);
   return chromeAndRubber(std::move(chrome), std::move(rubber));
}

Model buildFloorDrumLegs(float diameter, float headHeight, int legs)
{
   const float r = 0.5f * diameter;
   MeshData chrome, rubber;
   for (int i = 0; i < legs; i++)
   {
      const glm::vec3 out = radial(glm::two_pi<float>() * (i + 0.5f) / legs + glm::half_pi<float>());
      const glm::vec3 bracket = (r + 0.015f) * out;
      const glm::vec3 leg = (r + 0.03f) * out;
      chrome.append(extrudeBetween(bracketProfile, bracket - 0.075f * up, bracket - 0.125f * up, out));
      chrome.append(extrudeBetween(legProfile, leg - 0.05f * up, leg + (0.02f - headHeight) * up, out));
      rubber.append(footMesh(), glm::translate(glm::mat4(1), leg - headHeight * up));
   }
   return chromeAndRubber(std::move(chrome), std::move(rubber));
}

Model buildDrumStand(const DrumSpec &drum, float headHeight, float tilt, int legs)
{
   const float r = 0.5f * drum.diameter;
   const float bottom = headHeight - drum.depth;
   const float basketY = bottom - basketDrop;
   const float reach = r + 0.025f;
   const float knuckleY = basketY - jointDrop;

   MeshData chrome, rubber;
   tripod(chrome, rubber, legs, 0.25f, 0.3f, 0.15f);
   chrome.append(tube(0.0125f, 0.1f, knuckleY));
   chrome.append(tube(0.015f, knuckleY - 0.015f, knuckleY + 0.015f));

   MeshData basket, basketRubber;
   basket.append(tube(0.01f, knuckleY, basketY));
   basket.append(tube(0.02f, basketY - 0.02f, basketY + 0.02f));
   for (int i = 0; i < 3; i++)
   {
      const glm::vec3 out = radial(glm::two_pi<float>() * (i + 0.5f) / 3 + glm::half_pi<float>());
      const glm::vec3 elbow = reach * out + basketY * up;
      const glm::vec3 tip = reach * out + (bottom + 0.03f) * up;
      basket.append(extrudeBetween(braceProfile, 0.02f * out + basketY * up, elbow + 0.005f * out, up));
      basket.append(extrudeBetween(braceProfile, elbow - 0.005f * up, tip, out));
      basketRubber.append(tube(0.01f, 0, 0.02f), glm::translate(glm::mat4(1), tip - 0.01f * up));
   }
   const glm::mat4 tilted = tiltAbout(knuckleY, tilt);
   chrome.append(basket, tilted);
   rubber.append(basketRubber, tilted);
   return chromeAndRubber(std::move(chrome), std::move(rubber), glm::translate(glm::mat4(1), -headHeight * up));
}

glm::mat4 drumStandTilt(const DrumSpec &drum, float tilt)
{
   return tiltAbout(-drum.depth - basketDrop - jointDrop, tilt);
}
