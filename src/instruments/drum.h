#pragma once
#include "render/model.h"

//  sizes in meters
struct DrumSpec
{
   float diameter;
   float depth;
   int lugs;
};

//  modern drum
Model buildDrum(const DrumSpec &spec, const Material &finish, int note);
