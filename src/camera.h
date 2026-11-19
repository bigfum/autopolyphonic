#pragma once
#include <glm/glm.hpp>

//  orbit camera adapted from CSCIx229 ex13, with blender-style orbit/pan/zoom
struct Camera
{
   float th = 25;
   float ph = 20;
   float dim = 2.2f;
   float fov = 55;
   glm::vec3 target{0, 1.0f, 0.5f};

   glm::vec3 eye() const;
   glm::mat4 view() const;
   glm::mat4 projection(float asp) const;

   void orbit(float dx, float dy);
   void pan(float dx, float dy, float windowHeight);
   void zoom(float factor);
   void reset();
};
