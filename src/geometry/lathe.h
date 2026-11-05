#pragma once
#include "geometry/shapes.h"
#include <vector>

//  surface of revolution: profile points (radius, height) swept 360 degrees about the y axis.
//  walk the profile with the outside on your right (like bottom to top up an outer wall)
//  segments meeting at less than creaseAngle degrees get smooth shared normals.
MeshData lathe(const std::vector<glm::vec2> &profile, int segments, float creaseAngle = 30.0f);
