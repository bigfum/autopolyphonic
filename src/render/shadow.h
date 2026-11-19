#pragma once
#include "gl/shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class ShadowMap
{
public:
   explicit ShadowMap(int size = 2048);
   ~ShadowMap();
   ShadowMap(const ShadowMap &) = delete;
   ShadowMap &operator=(const ShadowMap &) = delete;

   const Shader &begin(const glm::vec3 &lightDirection, const glm::vec3 &center, float radius);
   void end();
   void apply(const Shader &shader, int unit) const;
   void drawDebug(int w, int h);

private:
   int size;
   GLuint fbo = 0;
   GLuint texture = 0;
   GLuint emptyVao = 0;
   glm::mat4 lightSpace{1};
   Shader depth;
   Shader view;
};
