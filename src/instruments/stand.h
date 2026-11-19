#pragma once
#include "render/model.h"

struct StandSpec
{
   float height; //  floor to the felt the cymbal rests on
   int legs = 3;
   float spread = 0.3f;
   float tilt = 0;
};

Model buildCymbalStand(const StandSpec &spec);
