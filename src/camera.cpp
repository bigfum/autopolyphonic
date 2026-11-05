#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Camera::eye() const
{
   const float t = glm::radians(th);
   const float p = glm::radians(ph);
   return target + 2 * dim * glm::vec3(-std::sin(t) * std::cos(p), std::sin(p), std::cos(t) * std::cos(p));
}

//  gluLookAt from ex13
glm::mat4 Camera::view() const
{
   return glm::lookAt(eye(), target, glm::vec3(0, 1, 0));
}

//  gluPerspective from ex13
glm::mat4 Camera::projection(float asp) const
{
   return glm::perspective(glm::radians(fov), asp, dim / 16, 16 * dim);
}
