#pragma once
#include <glm/glm.hpp>

//  shared helpers for the 2D profiles fed to lathe() (and extrude() next)

//  outward normal of the profile edge a -> b, with the outside on the right of the walk
inline glm::vec2 edgeNormal(const glm::vec2 &a, const glm::vec2 &b)
{
   const glm::vec2 d = b - a;
   return glm::normalize(glm::vec2(d.y, -d.x));
}

//  normal where two edges meet: averaged if the bend is gentle, otherwise the edge's own (a crease)
inline glm::vec2 blendNormal(const glm::vec2 &own, const glm::vec2 &neighbour, float cosCrease)
{
   return glm::dot(own, neighbour) >= cosCrease ? glm::normalize(own + neighbour) : own;
}
