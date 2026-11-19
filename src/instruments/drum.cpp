#include "instruments/drum.h"
#include "geometry/extrude.h"
#include "geometry/lathe.h"
#include "geometry/profile.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
   constexpr int segments = 48;
   constexpr float headThickness = 0.002f;
   constexpr float shellWall = 0.01f;

   //  measured out from the shell's outer radius
   constexpr float hoopInner = 0.005f;
   constexpr float hoopOuter = 0.01f;
   constexpr float flangeOuter = 0.02f;
   constexpr float rodOut = 0.015f;
   constexpr float lugOut = 0.01f;

   constexpr float hoopTop = 0.01f;
   constexpr float hoopBottom = -0.015f;
   constexpr float flangeTop = -0.01f;
   constexpr float lugTop = -0.025f;

   const std::vector<glm::vec2> lugProfile = roundedRect(0.015f, 0.02f, 0.005f);
   const std::vector<glm::vec2> rodProfile = roundedRect(0.005f, 0.005f, 0.002f);
   const std::vector<glm::vec2> boltProfile = roundedRect(0.01f, 0.01f, 0.002f);

   glm::vec3 radial(float a)
   {
      return {std::cos(a), 0, std::sin(a)};
   }

   MeshData headMesh(float r)
   {
      const float edge = r + hoopInner;
      return lathe({{0, -headThickness}, {edge, -headThickness}, {edge, 0}, {0, 0}}, segments);
   }

   MeshData hardwareMesh(float r, float depth, int lugs)
   {
      MeshData mesh;
      mesh.append(lathe({{r + hoopInner, hoopBottom},
                         {r + flangeOuter, hoopBottom},
                         {r + flangeOuter, flangeTop},
                         {r + hoopOuter, flangeTop},
                         {r + hoopOuter, hoopTop},
                         {r + hoopInner, hoopTop},
                         {r + hoopInner, hoopBottom}},
                        segments));

      const float lugLength = std::min(0.05f, 0.25f * depth);
      const glm::vec3 up(0, 1, 0);
      for (int i = 0; i < lugs; i++)
      {
         const glm::vec3 out = radial(glm::two_pi<float>() * (i + 0.5f) / lugs);
         const glm::vec3 lug = (r + lugOut) * out;
         const glm::vec3 rod = (r + rodOut) * out;
         mesh.append(extrudeBetween(lugProfile, lug + lugTop * up, lug + (lugTop - lugLength) * up, out));
         mesh.append(extrudeBetween(rodProfile, rod + flangeTop * up, rod + lugTop * up, out));
         mesh.append(extrudeBetween(boltProfile, rod, rod + flangeTop * up, out));
      }
      return mesh;
   }
}

Model buildDrum(const DrumSpec &spec, const Material &finish, int note)
{
   const float r = 0.5f * spec.diameter;
   const float depth = spec.depth;
   //  half a turn rather than a mirror, which would turn every triangle inside out
   const glm::mat4 flip = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, -depth, 0)), glm::pi<float>(),
                                      glm::vec3(1, 0, 0));

   const float shellTop = -headThickness;
   const float shellBottom = -depth + headThickness;
   MeshData shell = lathe({{r - shellWall, shellBottom},
                           {r, shellBottom},
                           {r, shellTop},
                           {r - shellWall, shellTop},
                           {r - shellWall, shellBottom}},
                          segments);

   MeshData resonantHead;
   resonantHead.append(headMesh(r), flip);

   MeshData hardware = hardwareMesh(r, depth, spec.lugs);
   hardware.append(hardwareMesh(r, depth, spec.lugs), flip);

   Model drum;
   drum.add(headMesh(r), materials::drumHead, glm::vec3(0), note);
   drum.add(std::move(resonantHead), materials::drumHead);
   drum.add(std::move(shell), finish);
   drum.add(std::move(hardware), materials::chrome);
   return drum;
}
