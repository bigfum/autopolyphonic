#include "gl/text.h"
#include "gl/font.h"
#include <cstdarg>
#include <cstdio>
#include <vector>

static constexpr int atlasWidth = 16 * fontWidth;
static constexpr int atlasHeight = 16 * fontHeight;

Text::Text()
    : shader("shaders/text.vert", "shaders/text.frag")
{
   std::vector<unsigned char> pixels(atlasWidth * atlasHeight);
   for (int c = 0; c < 256; c++)
      for (int row = 0; row < fontHeight; row++)
         for (int bit = 0; bit < fontWidth; bit++)
         {
            const int x = (c % 16) * fontWidth + bit;
            const int y = (c / 16) * fontHeight + row;
            pixels[y * atlasWidth + x] = (fontGlyphs[c][row] >> (7 - bit)) & 1 ? 255 : 0;
         }

   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
   glEnableVertexAttribArray(1);
   glBindVertexArray(0);
}

Text::~Text()
{
   glDeleteBuffers(1, &vbo);
   glDeleteVertexArrays(1, &vao);
   glDeleteTextures(1, &texture);
}

void Text::resize(int windowWidth, int drawableWidth, int drawableHeight)
{
   scale = windowWidth > 0 ? static_cast<float>(drawableWidth) / windowWidth : 1;
   width = drawableWidth;
   height = drawableHeight;
}

void Text::print(int x, int y, const char *format, ...)
{
   char buf[1024];
   va_list args;
   va_start(args, format);
   std::vsnprintf(buf, sizeof(buf), format, args);
   va_end(args);

   std::vector<float> verts;
   float px = x * scale;
   const float py = y * scale;
   const float w = fontWidth * scale;
   const float h = fontHeight * scale;
   for (const char *ch = buf; *ch; ch++)
   {
      const unsigned char c = static_cast<unsigned char>(*ch);
      const float u0 = static_cast<float>((c % 16) * fontWidth) / atlasWidth;
      const float v0 = static_cast<float>((c / 16) * fontHeight) / atlasHeight;
      const float u1 = u0 + static_cast<float>(fontWidth) / atlasWidth;
      const float v1 = v0 + static_cast<float>(fontHeight) / atlasHeight;
      const float quad[] = {
          px,
          py,
          u0,
          v0,
          px + w,
          py,
          u1,
          v0,
          px + w,
          py + h,
          u1,
          v1,
          px,
          py,
          u0,
          v0,
          px + w,
          py + h,
          u1,
          v1,
          px,
          py + h,
          u0,
          v1,
      };
      verts.insert(verts.end(), std::begin(quad), std::end(quad));
      px += fontAdvance * scale;
   }

   glDisable(GL_DEPTH_TEST);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   shader.use();
   shader.set("uViewport", glm::vec2(width, height));
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
   glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size() / 4));
}
