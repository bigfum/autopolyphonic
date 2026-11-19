#pragma once
#include "gl/shader.h"
#include <glad/glad.h>

class Text
{
public:
   Text();
   ~Text();
   Text(const Text &) = delete;
   Text &operator=(const Text &) = delete;

   void resize(int windowWidth, int drawableWidth, int drawableHeight);

   void print(int x, int y, const char *format, ...) __attribute__((format(printf, 4, 5)));

private:
   Shader shader;
   GLuint texture = 0;
   GLuint vao = 0;
   GLuint vbo = 0;
   float scale = 1;
   int width = 1;
   int height = 1;
};
