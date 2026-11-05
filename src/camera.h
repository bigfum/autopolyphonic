#pragma once
#include <glm/glm.hpp>

//  orbit camera adapted from CSCIx229 ex13
struct Camera
{
   float th = 30;
   float ph = 25;
   float dim = 2.5f;
   float fov = 55;
   glm::vec3 target{0, 0.4f, 0};

   glm::vec3 eye() const;
   glm::mat4 view() const;
   glm::mat4 projection(float asp) const;
};
