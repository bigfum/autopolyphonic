#pragma once
#include <glm/glm.hpp>
#include <vector>

//  helpers for 2D profiles fed to lathe() and extrude()

//  outward normal of the profile edge a -> b, with the outside on the right of the walk
inline glm::vec2 edgeNormal(const glm::vec2 &a, const glm::vec2 &b)
{
   const glm::vec2 d = b - a;
   return glm::normalize(glm::vec2(d.y, -d.x));
}

//  normal avgd if bend gentle, otherwise keep edge's own (crease)
inline glm::vec2 blendNormal(const glm::vec2 &own, const glm::vec2 &neighbour, float cosCrease)
{
   return glm::dot(own, neighbour) >= cosCrease ? glm::normalize(own + neighbour) : own;
}

//  counter-clockwise rounded rectangle centered on the origin (default 2 segments per corner)
std::vector<glm::vec2> roundedRect(float width, float height, float radius, int cornerSegments = 2);
