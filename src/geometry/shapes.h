#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
   glm::vec3 position;
   glm::vec3 normal;
};

struct MeshData
{
   std::vector<Vertex> vertices;
   std::vector<unsigned> indices;

   void append(const MeshData &other, const glm::mat4 &m = glm::mat4(1));
};

MeshData makePlane(float size);

MeshData makeSphere(float radius, int rings, int segments);
