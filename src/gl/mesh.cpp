#include "gl/mesh.h"
#include <cstddef>
#include <utility>

Mesh::Mesh(const MeshData &data)
    : count(static_cast<GLsizei>(data.indices.size()))
{
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned), data.indices.data(), GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
   glEnableVertexAttribArray(1);
   glBindVertexArray(0);
}

Mesh::~Mesh()
{
   release();
}

Mesh::Mesh(Mesh &&other) noexcept
    : vao(std::exchange(other.vao, 0)),
      vbo(std::exchange(other.vbo, 0)),
      ebo(std::exchange(other.ebo, 0)),
      count(std::exchange(other.count, 0))
{
}

Mesh &Mesh::operator=(Mesh &&other) noexcept
{
   if (this != &other)
   {
      release();
      vao = std::exchange(other.vao, 0);
      vbo = std::exchange(other.vbo, 0);
      ebo = std::exchange(other.ebo, 0);
      count = std::exchange(other.count, 0);
   }
   return *this;
}

void Mesh::release()
{
   glDeleteBuffers(1, &ebo);
   glDeleteBuffers(1, &vbo);
   glDeleteVertexArrays(1, &vao);
}

void Mesh::draw() const
{
   glBindVertexArray(vao);
   glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}
