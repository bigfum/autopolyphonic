#include "geometry/shapes.h"
#include <glm/gtc/constants.hpp>

void MeshData::append(const MeshData &other, const glm::mat4 &m)
{
   const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m)));
   const unsigned base = static_cast<unsigned>(vertices.size());
   for (const Vertex &v : other.vertices)
      vertices.push_back({glm::vec3(m * glm::vec4(v.position, 1)), glm::normalize(normalMatrix * v.normal)});
   for (unsigned i : other.indices)
      indices.push_back(base + i);
}

MeshData makePlane(float size)
{
   const float h = 0.5f * size;
   const glm::vec3 up(0, 1, 0);
   MeshData mesh;
   mesh.vertices = {
       {{-h, 0, h}, up},
       {{h, 0, h}, up},
       {{h, 0, -h}, up},
       {{-h, 0, -h}, up},
   };
   mesh.indices = {0, 1, 2, 0, 2, 3};
   return mesh;
}

MeshData makeSphere(float radius, int rings, int segments)
{
   MeshData mesh;
   for (int i = 0; i <= rings; i++)
   {
      const float ph = glm::pi<float>() * (static_cast<float>(i) / rings - 0.5f);
      for (int j = 0; j <= segments; j++)
      {
         const float th = glm::two_pi<float>() * j / segments;
         const glm::vec3 n(std::cos(ph) * std::sin(th), std::sin(ph), std::cos(ph) * std::cos(th));
         mesh.vertices.push_back({radius * n, n});
      }
   }

   for (int i = 0; i < rings; i++)
      for (int j = 0; j < segments; j++)
      {
         const unsigned a = i * (segments + 1) + j;
         const unsigned b = a + 1;
         const unsigned c = a + segments + 1;
         const unsigned d = c + 1;
         mesh.indices.insert(mesh.indices.end(), {a, b, d, a, d, c});
      }
   return mesh;
}
