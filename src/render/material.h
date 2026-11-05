#pragma once
#include "gl/shader.h"
#include <glm/glm.hpp>

struct Material
{
   glm::vec3 albedo;
   float specular;
   float shininess;

   void apply(const Shader &shader) const
   {
      shader.set("uAlbedo", albedo);
      shader.set("uSpecular", specular);
      shader.set("uShininess", shininess);
   }
};

//  every material in the scene
namespace materials
{
   inline const Material floor{{0.2f, 0.2f, 0.2f}, 0.1f, 16.0f};
   inline const Material plastic{{1.0f, 0.0f, 0.0f}, 0.5f, 64.0f};
}
