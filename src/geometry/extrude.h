#pragma once
#include "geometry/shapes.h"
#include <vector>

//  prism: closed counter-clockwise outline in the xy plane pushed along +z from 0 to depth, with caps.
//  the outline may be concave but must not cross itself.
//  side edges meeting at less than creaseAngle degrees get smooth shared normals
MeshData extrude(const std::vector<glm::vec2> &profile, float depth, float creaseAngle = 60.0f);

//  extrude along the segment from -> to, keeping the profile's y axis as close to up as possible.
//  for rails, beams and rods.
MeshData extrudeBetween(const std::vector<glm::vec2> &profile, const glm::vec3 &from, const glm::vec3 &to,
                        const glm::vec3 &up = glm::vec3(0, 1, 0));
