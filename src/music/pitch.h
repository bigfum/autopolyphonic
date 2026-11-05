#pragma once
#include <cmath>

//  equal-tempered frequency of a MIDI note (A4 = 69 = 440 Hz)
inline float midiToHz(int note)
{
   return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}

//  true for the white keys of a piano (C D E F G A B)
inline bool isNatural(int note)
{
   switch (note % 12)
   {
   case 1:
   case 3:
   case 6:
   case 8:
   case 10:
      return false;
   default:
      return true;
   }
}
