#pragma once
#include "instruments/drum.h"
#include "render/model.h"

struct StandSpec
{
   float height; //  floor to the felt the cymbal rests on
   int legs = 3;
   float spread = 0.3f;
   float tilt = 0;
};

Model buildCymbalStand(const StandSpec &spec);

//  straight legs dropping from brackets on the shell
Model buildFloorDrumLegs(float diameter, float headHeight, int legs = 3);

//  tripod and pipe up to a basket that grips the bottom hoop, tilted toward +z by tilt degrees
Model buildDrumStand(const DrumSpec &drum, float headHeight, float tilt = 0, int legs = 3);

glm::mat4 drumStandTilt(const DrumSpec &drum, float tilt);
