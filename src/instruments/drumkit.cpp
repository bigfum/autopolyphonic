#include "instruments/drumkit.h"
#include "instruments/cymbal.h"
#include "instruments/drum.h"
#include "instruments/stand.h"
#include <glm/gtc/matrix_transform.hpp>

namespace
{
   const glm::vec3 throne(0, 0, 0.8f);

   glm::mat4 at(const glm::mat4 &m, float x, float y, float z)
   {
      return glm::translate(m, glm::vec3(x, y, z));
   }

   //  at (x, y, z), turned about y so the piece's +z (its tilt direction) points at the throne,
   //  plus an optional extra turn in degrees
   glm::mat4 facing(const glm::mat4 &m, float x, float y, float z, float turn = 0)
   {
      const float yaw = std::atan2(throne.x - x, throne.z - z) + glm::radians(turn);
      return glm::rotate(at(m, x, y, z), yaw, glm::vec3(0, 1, 0));
   }

   glm::mat4 tilted(const glm::mat4 &m, float degrees)
   {
      return glm::rotate(m, glm::radians(degrees), glm::vec3(1, 0, 0));
   }

   void addCymbal(Stage &stage, const glm::mat4 &base, float diameter, const StandSpec &stand, int note)
   {
      stage.add(buildCymbalStand(stand), base);
      stage.add(buildCymbal(diameter, note), tilted(at(base, 0, stand.height, 0), stand.tilt));
   }
}

void addDrumKit(Stage &stage, const glm::mat4 &kit)
{
   const Material &finish = materials::lacquer;

   const DrumSpec kick{0.55f, 0.45f, 10};
   stage.add(buildDrum(kick, finish, 36), tilted(facing(kit, 0, 0.3f, 0.2f), 90));

   const DrumSpec snare{0.35f, 0.15f, 10};
   const float snareHeight = 0.65f;
   const float snareTilt = 10;
   const glm::mat4 snareHead = facing(kit, -0.275f, snareHeight, 0.4f);
   stage.add(buildDrum(snare, finish, 38), snareHead * drumStandTilt(snare, snareTilt));
   stage.add(buildDrumStand(snare, snareHeight, snareTilt), snareHead);

   const DrumSpec highTom{0.25f, 0.2f, 6};
   const DrumSpec midTom{0.3f, 0.25f, 6};
   stage.add(buildDrum(highTom, finish, 50), tilted(facing(kit, -0.17f, 0.85f, 0.05f), 15));
   stage.add(buildDrum(midTom, finish, 47), tilted(facing(kit, 0.17f, 0.9f, 0.05f), 15));

   const DrumSpec floorTom{0.4f, 0.4f, 8};
   const float floorTomHeight = 0.6f;
   const glm::mat4 floorTomHead = facing(kit, 0.5f, floorTomHeight, 0.35f);
   stage.add(buildDrum(floorTom, finish, 43), floorTomHead);
   stage.add(buildFloorDrumLegs(floorTom.diameter, floorTomHeight), floorTomHead);

   addCymbal(stage, facing(kit, -0.45f, 0, 0.0f), 0.45f, {1.2f, 3, 0.3f, 15}, 49);
   addCymbal(stage, facing(kit, 0.5f, 0, 0.0f), 0.5f, {1.1f, 3, 0.3f, 10}, 51);

   const float hatDiameter = 0.35f;
   const float hatHeight = 0.85f;
   const float hatGap = 2 * cymbalEdgeDrop(hatDiameter) + 0.01f;
   const glm::mat4 hat = facing(kit, -0.65f, 0, 0.35f);
   stage.add(buildCymbalStand({hatHeight}), hat);
   stage.add(buildCymbal(hatDiameter, -1), tilted(at(hat, 0, hatHeight, 0), 180));
   stage.add(buildCymbal(hatDiameter, 42), at(hat, 0, hatHeight + hatGap, 0));
}
