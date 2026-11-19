#include "instruments/stand.h"
#include "geometry/extrude.h"
#include "geometry/lathe.h"
#include "geometry/profile.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
   constexpr int segments = 24;
   constexpr float hubY = 0.4f;
   constexpr float braceY = 0.2f;
   constexpr float clampY = 0.8f;

   const std::vector<glm::vec2> legProfile = roundedRect(0.015f, 0.015f, 0.005f);
   const std::vector<glm::vec2> braceProfile = roundedRect(0.01f, 0.01f, 0.004f);

   MeshData tube(float r, float y0, float y1)
   {
      return lathe({{0, y0}, {r, y0}, {r, y1}, {0, y1}}, segments);
   }
}

Model buildCymbalStand(const StandSpec &spec)
{
   const glm::vec3 up(0, 1, 0);
   const float knuckleY = spec.height - 0.05f;
   const glm::mat4 tilted = glm::rotate(glm::translate(glm::mat4(1), spec.height * up), glm::radians(spec.tilt),
                                        glm::vec3(1, 0, 0));

   MeshData chrome;
   chrome.append(tube(0.0125f, 0.15f, clampY));
   chrome.append(tube(0.02f, clampY - 0.02f, clampY + 0.02f));
   chrome.append(tube(0.01f, clampY, knuckleY));
   chrome.append(tube(0.02f, hubY - 0.03f, hubY + 0.03f));
   chrome.append(tube(0.02f, braceY - 0.02f, braceY + 0.02f));
   chrome.append(tube(0.015f, knuckleY - 0.015f, knuckleY + 0.015f));
   chrome.append(tube(0.005f, knuckleY, spec.height - 0.01f));
   chrome.append(tube(0.005f, -0.01f, 0.04f), tilted);

   MeshData rubber;
   rubber.append(tube(0.02f, -0.01f, 0), tilted);
   for (int i = 0; i < spec.legs; i++)
   {
      const float a = glm::two_pi<float>() * i / spec.legs + glm::half_pi<float>();
      const glm::vec3 out(std::cos(a), 0, std::sin(a));
      const glm::vec3 top = 0.02f * out + hubY * up;
      const glm::vec3 foot = spec.spread * out + 0.02f * up;
      chrome.append(extrudeBetween(legProfile, top, foot, out));
      chrome.append(extrudeBetween(braceProfile, 0.02f * out + braceY * up, 0.5f * (top + foot), out));
      rubber.append(lathe({{0, 0}, {0.015f, 0}, {0.015f, 0.02f}, {0, 0.035f}}, 16),
                    glm::translate(glm::mat4(1), glm::vec3(foot.x, 0, foot.z)));
   }

   Model stand;
   stand.add(std::move(chrome), materials::chrome);
   stand.add(std::move(rubber), materials::rubber);
   return stand;
}
