#pragma once
#include "gl/shader.h"
#include <glm/glm.hpp>

struct DirectionalLight
{
   glm::vec3 direction;
   glm::vec3 color;
   glm::vec3 ambient;

   void apply(const Shader &shader) const
   {
      shader.set("uLightDirection", glm::normalize(direction));
      shader.set("uLightColor", color);
      shader.set("uAmbient", ambient);
   }
};
