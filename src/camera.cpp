#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

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

void Camera::orbit(float dx, float dy)
{
   th += 0.3f * dx;
   ph = std::clamp(ph + 0.3f * dy, -85.0f, 85.0f);
}

void Camera::pan(float dx, float dy, float windowHeight)
{
   const glm::vec3 forward = glm::normalize(target - eye());
   const glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
   const glm::vec3 up = glm::cross(right, forward);
   const float unitsPerPixel = 2 * (2 * dim) * std::tan(glm::radians(0.5f * fov)) / windowHeight;
   target += unitsPerPixel * (-dx * right + dy * up);
}

void Camera::zoom(float factor)
{
   dim = std::clamp(dim * factor, 0.1f, 20.0f);
}

void Camera::reset()
{
   *this = Camera();
}
