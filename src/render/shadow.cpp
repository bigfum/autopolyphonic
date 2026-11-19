#include "render/shadow.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

ShadowMap::ShadowMap(int size)
    : size(size), depth("shaders/depth.vert", "shaders/depth.frag"), view("shaders/depthview.vert", "shaders/depthview.frag")
{
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
   const float border[4] = {1, 1, 1, 1};
   glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

   glGenFramebuffers(1, &fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
   {
      std::fprintf(stderr, "shadow map framebuffer incomplete\n");
      std::exit(1);
   }
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   glGenVertexArrays(1, &emptyVao);
}

ShadowMap::~ShadowMap()
{
   glDeleteVertexArrays(1, &emptyVao);
   glDeleteFramebuffers(1, &fbo);
   glDeleteTextures(1, &texture);
}

const Shader &ShadowMap::begin(const glm::vec3 &lightDirection, const glm::vec3 &center, float radius)
{
   const glm::vec3 eye = center + radius * glm::normalize(lightDirection);
   lightSpace = glm::ortho(-radius, radius, -radius, radius, 0.0f, 2 * radius) *
                glm::lookAt(eye, center, glm::vec3(0, 1, 0));

   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   glViewport(0, 0, size, size);
   glClear(GL_DEPTH_BUFFER_BIT);
   glCullFace(GL_FRONT);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1, 1);

   depth.use();
   depth.set("uLightSpace", lightSpace);
   return depth;
}

void ShadowMap::end()
{
   glDisable(GL_POLYGON_OFFSET_FILL);
   glCullFace(GL_BACK);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::apply(const Shader &shader, int unit) const
{
   glActiveTexture(GL_TEXTURE0 + unit);
   glBindTexture(GL_TEXTURE_2D, texture);
   glActiveTexture(GL_TEXTURE0);
   shader.set("uShadowMap", unit);
   shader.set("uLightSpace", lightSpace);
}

void ShadowMap::drawDebug(int w, int h)
{
   const int side = std::min(w, h) / 3;
   glViewport(w - side, 0, side, side);
   glDisable(GL_DEPTH_TEST);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
   view.use();
   view.set("uDepth", 0);
   glBindVertexArray(emptyVao);
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   glBindVertexArray(0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

   glViewport(0, 0, w, h);
   glEnable(GL_DEPTH_TEST);
}
