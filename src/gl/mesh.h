#pragma once
#include "geometry/shapes.h"
#include <glad/glad.h>

class Mesh
{
public:
   explicit Mesh(const MeshData &data);
   ~Mesh();
   Mesh(const Mesh &) = delete;
   Mesh &operator=(const Mesh &) = delete;

   void draw() const;

private:
   GLuint vao = 0;
   GLuint vbo = 0;
   GLuint ebo = 0;
   GLsizei count = 0;
};
