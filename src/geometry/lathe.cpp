#include "geometry/lathe.h"
#include "geometry/profile.h"
#include <glm/gtc/constants.hpp>

MeshData lathe(const std::vector<glm::vec2> &profile, int segments, float creaseAngle)
{
   MeshData mesh;
   const size_t n = profile.size();
   if (n < 2)
      return mesh;
   const float cosCrease = std::cos(glm::radians(creaseAngle));

   std::vector<glm::vec2> normals(n - 1);
   for (size_t i = 0; i + 1 < n; i++)
      normals[i] = edgeNormal(profile[i], profile[i + 1]);

   //  each profile segment gets its own two rings, so creases stay sharp
   for (size_t i = 0; i + 1 < n; i++)
   {
      const glm::vec2 n0 = i > 0 ? blendNormal(normals[i], normals[i - 1], cosCrease) : normals[i];
      const glm::vec2 n1 = i + 2 < n ? blendNormal(normals[i], normals[i + 1], cosCrease) : normals[i];
      const glm::vec2 &p0 = profile[i];
      const glm::vec2 &p1 = profile[i + 1];

      const unsigned base = static_cast<unsigned>(mesh.vertices.size());
      for (int j = 0; j <= segments; j++)
      {
         //  rotating (r, y) about y gives (r cos, y, r sin); the normal rotates the same way
         const float a = glm::two_pi<float>() * j / segments;
         const float c = std::cos(a);
         const float s = std::sin(a);
         mesh.vertices.push_back({{p0.x * c, p0.y, p0.x * s}, {n0.x * c, n0.y, n0.x * s}});
         mesh.vertices.push_back({{p1.x * c, p1.y, p1.x * s}, {n1.x * c, n1.y, n1.x * s}});
      }
      for (int j = 0; j < segments; j++)
      {
         const unsigned a = base + 2 * j; //  ring start, column j
         const unsigned b = a + 2;        //  ring start, column j + 1
         const unsigned c = a + 1;        //  ring end, column j
         const unsigned d = a + 3;        //  ring end, column j + 1
         //  a ring on the axis collapses to a point, leaving one real triangle per quad
         if (p0.x != 0)
            mesh.indices.insert(mesh.indices.end(), {a, c, b});
         if (p1.x != 0)
            mesh.indices.insert(mesh.indices.end(), {b, c, d});
      }
   }
   return mesh;
}
