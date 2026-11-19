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
   inline const Material floor{{0.005f, 0.005f, 0.005f}, 0.1f, 16.0f};
   inline const Material stage{{0.03f, 0.03f, 0.035f}, 0.2f, 32.0f};
   inline const Material plastic{{1.0f, 0.0f, 0.0f}, 0.5f, 64.0f};
   inline const Material brass{{0.5f, 0.3f, 0.1f}, 0.9f, 96.0f};
   inline const Material darkWood{{0.06f, 0.03f, 0.02f}, 0.3f, 40.0f};
   inline const Material rosewood{{0.2f, 0.045f, 0.02f}, 0.35f, 48.0f};
   inline const Material drumHead{{0.75f, 0.73f, 0.68f}, 0.15f, 16.0f};
   inline const Material lacquer{{0.22f, 0.015f, 0.025f}, 0.8f, 96.0f};
   inline const Material chrome{{0.5f, 0.5f, 0.53f}, 1.0f, 128.0f};
   inline const Material bronze{{0.55f, 0.36f, 0.1f}, 0.9f, 64.0f};
   inline const Material rubber{{0.02f, 0.02f, 0.02f}, 0.1f, 8.0f};
}
