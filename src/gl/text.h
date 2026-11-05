#pragma once
#include "gl/shader.h"
#include <glad/glad.h>

//  on-screen text overlay; core profile replacement for the examples' glWindowPos2i + Print
class Text
{
public:
   Text();
   ~Text();
   Text(const Text &) = delete;
   Text &operator=(const Text &) = delete;

   //  window size in points and drawable size in pixels (they differ on HiDPI screens)
   void resize(int windowWidth, int drawableWidth, int drawableHeight);

   //  printf-style text with its bottom-left corner at (x, y) in window points
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
