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
};

MeshData makePlane(float size);

MeshData makeSphere(float radius, int rings, int segments);
