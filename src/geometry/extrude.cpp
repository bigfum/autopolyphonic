#include "geometry/extrude.h"
#include "geometry/profile.h"
#include <cstdio>
#include <cstdlib>
#include <numeric>

//  twice the signed area of triangle abc: positive when counter-clockwise
static float cross(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c)
{
   return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

static bool insideTriangle(const glm::vec2 &p, const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c)
{
   return cross(a, b, p) > 0 && cross(b, c, p) > 0 && cross(c, a, p) > 0;
}

//  ear clipping: repeatedly cut off a convex corner whose triangle holds no other outline point
static std::vector<unsigned> triangulate(const std::vector<glm::vec2> &poly)
{
   std::vector<unsigned> result;
   std::vector<unsigned> remaining(poly.size());
   std::iota(remaining.begin(), remaining.end(), 0);

   while (remaining.size() > 3)
   {
      const size_t m = remaining.size();
      bool clipped = false;
      for (size_t i = 0; i < m && !clipped; i++)
      {
         const unsigned ia = remaining[(i + m - 1) % m];
         const unsigned ib = remaining[i];
         const unsigned ic = remaining[(i + 1) % m];
         const float area = cross(poly[ia], poly[ib], poly[ic]);
         //  straight-through point adds nothing: drop it
         if (area == 0)
         {
            remaining.erase(remaining.begin() + i);
            clipped = true;
            break;
         }
         //  reflex (concave) corner can't be an ear
         if (area < 0)
            continue;
         bool blocked = false;
         for (unsigned k : remaining)
            if (k != ia && k != ib && k != ic && insideTriangle(poly[k], poly[ia], poly[ib], poly[ic]))
            {
               blocked = true;
               break;
            }
         if (blocked)
            continue;
         result.insert(result.end(), {ia, ib, ic});
         remaining.erase(remaining.begin() + i);
         clipped = true;
      }
      if (!clipped)
      {
         std::fprintf(stderr, "extrude: outline is not a simple counter-clockwise polygon\n");
         std::exit(1);
      }
   }
   result.insert(result.end(), remaining.begin(), remaining.end());
   return result;
}

MeshData extrude(const std::vector<glm::vec2> &profile, float depth, float creaseAngle)
{
   MeshData mesh;
   const size_t n = profile.size();
   const float cosCrease = std::cos(glm::radians(creaseAngle));

   std::vector<glm::vec2> normals(n);
   for (size_t i = 0; i < n; i++)
      normals[i] = edgeNormal(profile[i], profile[(i + 1) % n]);

   //  side walls: one quad per outline edge, with its own vertices so creases stay sharp
   for (size_t i = 0; i < n; i++)
   {
      const glm::vec2 &p0 = profile[i];
      const glm::vec2 &p1 = profile[(i + 1) % n];
      const glm::vec2 n0 = blendNormal(normals[i], normals[(i + n - 1) % n], cosCrease);
      const glm::vec2 n1 = blendNormal(normals[i], normals[(i + 1) % n], cosCrease);

      const unsigned base = static_cast<unsigned>(mesh.vertices.size());
      mesh.vertices.push_back({{p0, 0}, {n0, 0}});
      mesh.vertices.push_back({{p1, 0}, {n1, 0}});
      mesh.vertices.push_back({{p1, depth}, {n1, 0}});
      mesh.vertices.push_back({{p0, depth}, {n0, 0}});
      mesh.indices.insert(mesh.indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
   }

   //  caps: the front (z = depth) keeps the outline's winding, the back (z = 0) is reversed
   const std::vector<unsigned> tris = triangulate(profile);
   for (int cap = 0; cap < 2; cap++)
   {
      const bool front = cap == 0;
      const unsigned base = static_cast<unsigned>(mesh.vertices.size());
      for (const glm::vec2 &p : profile)
         mesh.vertices.push_back({{p, front ? depth : 0}, {0, 0, front ? 1 : -1}});
      for (size_t t = 0; t < tris.size(); t += 3)
      {
         if (front)
            mesh.indices.insert(mesh.indices.end(), {base + tris[t], base + tris[t + 1], base + tris[t + 2]});
         else
            mesh.indices.insert(mesh.indices.end(), {base + tris[t], base + tris[t + 2], base + tris[t + 1]});
      }
   }
   return mesh;
}

MeshData extrudeBetween(const std::vector<glm::vec2> &profile, const glm::vec3 &from, const glm::vec3 &to,
                        const glm::vec3 &up)
{
   const glm::vec3 axis = to - from;
   const float length = glm::length(axis);
   const glm::vec3 forward = axis / length;
   //  if the segment points along up, any other reference will do
   const glm::vec3 reference = std::abs(glm::dot(forward, up)) > 0.999f ? glm::vec3(1, 0, 0) : up;
   const glm::vec3 y = glm::normalize(reference - forward * glm::dot(reference, forward));
   const glm::vec3 x = glm::cross(y, forward);

   //  right-handed basis (x cross y = forward), so the winding survives the transform
   const glm::mat4 m(glm::vec4(x, 0), glm::vec4(y, 0), glm::vec4(forward, 0), glm::vec4(from, 1));
   const glm::mat3 rotate(m);
   MeshData mesh = extrude(profile, length);
   for (Vertex &v : mesh.vertices)
   {
      v.position = glm::vec3(m * glm::vec4(v.position, 1));
      v.normal = rotate * v.normal;
   }
   return mesh;
}
