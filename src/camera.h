#pragma once
#include <glm/glm.hpp>

//  orbit camera adapted from CSCIx229 ex13
struct Camera
{
   float th = 25;
   float ph = 20;
   float dim = 1.7f;
   float fov = 55;
   glm::vec3 target{0, 0.75f, 0};

   glm::vec3 eye() const;
   glm::mat4 view() const;
   glm::mat4 projection(float asp) const;
};
