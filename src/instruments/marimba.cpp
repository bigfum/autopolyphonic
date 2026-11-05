#include "instruments/marimba.h"
#include "geometry/extrude.h"
#include "geometry/lathe.h"
#include "geometry/profile.h"
#include "music/pitch.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
   constexpr float barTopHeight = 0.92f; //  top of the natural row
   constexpr float accidentalRaise = 0.025f;
   constexpr float naturalSpacing = 0.068f; //  center to center along x
   constexpr float lowBarLength = 0.78f;
   constexpr float rowGap = 0.02f;        //  between the inner ends of the two rows
   constexpr float nodeFraction = 0.224f; //  vibration nodes of a free bar, from each end
   constexpr float cordGap = 0.008f;      //  bars float on cord above the rails
   constexpr float speedOfSound = 343.0f; //  m/s
   constexpr float endPad = 0.07f;        //  frame overhang past the end bars

   const std::vector<glm::vec2> railProfile = roundedRect(0.022f, 0.04f, 0.005f);
   const std::vector<glm::vec2> beamProfile = roundedRect(0.03f, 0.05f, 0.007f);
   const std::vector<glm::vec2> rodProfile = roundedRect(0.028f, 0.028f, 0.013f);

   //  side view of a bar (length along x, thickness along y), counter-clockwise,
   //  with the arched undercut between the nodes (this tunes a bar irl)
   std::vector<glm::vec2> barProfile(float length, float thickness, float archDepth)
   {
      const float hl = 0.5f * length;
      const float ht = 0.5f * thickness;
      const float chamfer = 0.003f;
      const float archHalf = (0.5f - nodeFraction) * length;
      const int archSteps = 16;

      std::vector<glm::vec2> points = {{-hl + chamfer, -ht}};
      for (int k = 0; k <= archSteps; k++)
      {
         const float x = -archHalf + 2 * archHalf * k / archSteps;
         const float lift = 0.5f * (1 + std::cos(glm::pi<float>() * x / archHalf));
         points.push_back({x, -ht + archDepth * lift});
      }
      points.insert(points.end(), {{hl - chamfer, -ht},
                                   {hl, -ht + chamfer},
                                   {hl, ht - chamfer},
                                   {hl - chamfer, ht},
                                   {-hl + chamfer, ht},
                                   {-hl, ht - chamfer},
                                   {-hl, -ht + chamfer}});
      return points;
   }

   //  bar mesh around its center: length along z (toward the player), width along x
   MeshData barMesh(float length, float width, float thickness, float archDepth)
   {
      //  profile x -> world z, profile y -> world y, extrusion -> world -x (keeps the basis right-handed)
      const glm::mat4 m(glm::vec4(0, 0, 1, 0), glm::vec4(0, 1, 0, 0), glm::vec4(-1, 0, 0, 0),
                        glm::vec4(0.5f * width, 0, 0, 1));
      MeshData mesh;
      //  30 degrees keeps the 45 degree chamfers as creases (so the top stays flat) while the
      //  arch's small steps still shade smoothly
      mesh.append(extrude(barProfile(length, thickness, archDepth), width, 30.0f), m);
      return mesh;
   }

   //  quarter-wave tube closed at the bottom, open top at y = 0
   MeshData resonatorMesh(float length, float radius)
   {
      const float wall = 0.0015f;
      return lathe({{0, -length},
                    {radius, -length},
                    {radius, 0},
                    {radius - wall, 0},
                    {radius - wall, -length + wall},
                    {0, -length + wall}},
                   24);
   }

   //  turned wooden leg from the floor up to height
   MeshData legMesh(float h)
   {
      return lathe({{0, 0},
                    {0.03f, 0},
                    {0.032f, 0.015f},
                    {0.026f, 0.035f},
                    {0.019f, 0.07f},
                    {0.017f, h - 0.1f},
                    {0.024f, h - 0.075f},
                    {0.024f, h - 0.055f},
                    {0.02f, h - 0.04f},
                    {0.02f, h},
                    {0, h}},
                   20);
   }

   //  least-squares cubic v(x), so a rail follows the trend of the node points instead of every kink
   struct Cubic
   {
      glm::vec4 c;
      float scale; //  x is divided by this to keep the fit well conditioned

      Cubic(const std::vector<float> &xs, const std::vector<float> &vs)
      {
         scale = 0;
         for (float x : xs)
            scale = std::max(scale, std::abs(x));
         //  normal equations (A^T A) c = A^T v with A's rows = (1, u, u^2, u^3)
         glm::mat4 ata(0);
         glm::vec4 atv(0);
         for (size_t i = 0; i < xs.size(); i++)
         {
            const float u = xs[i] / scale;
            const glm::vec4 row(1, u, u * u, u * u * u);
            ata += glm::outerProduct(row, row);
            atv += row * vs[i];
         }
         c = glm::inverse(ata) * atv;
      }

      float operator()(float x) const
      {
         const float u = x / scale;
         return c[0] + u * (c[1] + u * (c[2] + u * c[3]));
      }
   };
} // namespace

Marimba buildMarimba(int lowNote, int highNote)
{
   Marimba marimba;
   const float lowFreq = midiToHz(lowNote);

   //  naturals take consecutive slots; an accidental sits halfway past the natural below it
   float slot = -1;
   for (int note = lowNote; note <= highNote; note++)
   {
      const bool natural = isNatural(note);
      if (natural)
         slot += 1;
      const float t = static_cast<float>(note - lowNote) / (highNote - lowNote);
      const float freq = midiToHz(note);

      //  a free bar's frequency goes as 1/length^2, so length ~ 1/sqrt(f)
      const float length = lowBarLength * std::sqrt(lowFreq / freq);
      const float width = glm::mix(0.062f, 0.042f, t);
      const float thickness = glm::mix(0.024f, 0.018f, t);
      const float archDepth = glm::mix(0.45f, 0.2f, t) * thickness;

      const float x = (natural ? slot : slot + 0.5f) * naturalSpacing;
      const float side = natural ? 1.0f : -1.0f;
      const float y = barTopHeight - 0.5f * thickness + (natural ? 0 : accidentalRaise);
      const float z = side * (0.5f * rowGap + 0.5f * length);
      marimba.bars.push_back({note, {x, y, z}, thickness, barMesh(length, width, thickness, archDepth)});

      //  resonator: a quarter wavelength of the bar's pitch, hung just under the bar
      const float barBottom = y - 0.5f * thickness;
      const float tubeLength = std::max(speedOfSound / (4 * freq), 0.05f);
      const float tubeRadius = glm::mix(0.03f, 0.015f, t);
      marimba.resonators.append(resonatorMesh(tubeLength, tubeRadius),
                                glm::translate(glm::mat4(1), glm::vec3(x, barBottom - 0.03f, z)));
   }

   //  center the keyboard on x
   const float shift = -0.5f * (marimba.bars.front().center.x + marimba.bars.back().center.x);
   for (Marimba::Bar &b : marimba.bars)
      b.center.x += shift;
   for (Vertex &v : marimba.resonators.vertices)
      v.position.x += shift;

   //  two rails per row under the nodal points, running past the end bars into the end beams
   const float lowX = marimba.bars.front().center.x - endPad;
   const float highX = marimba.bars.back().center.x + endPad;
   float endZ[2][2] = {}; //  [row][low/high end] z of the outer rail
   float railY[2] = {};   //  [row] height of the rail centers at the ends
   for (int row = 0; row < 2; row++)
   {
      const bool natural = row == 0;
      const float side = natural ? 1.0f : -1.0f;
      for (float fraction : {nodeFraction, 1 - nodeFraction})
      {
         //  each bar's node point; E-F and B-C are a semitone apart instead of two, and the
         //  accidentals have gaps, so these zigzag slightly and can't be used as the path directly
         std::vector<float> xs, ys, zs;
         for (const Marimba::Bar &b : marimba.bars)
         {
            if (isNatural(b.note) != natural)
               continue;
            const float length = 2 * (std::abs(b.center.z) - 0.5f * rowGap);
            xs.push_back(b.center.x);
            ys.push_back(b.center.y - 0.5f * b.thickness - cordGap - 0.02f);
            zs.push_back(side * (0.5f * rowGap + fraction * length));
         }
         //  smooth curves through them stay within 6 mm of every node, well inside the rail's width
         const Cubic railHeight(xs, ys);
         const Cubic railDepth(xs, zs);
         const int samples = 64;
         std::vector<glm::vec3> points;
         for (int i = 0; i <= samples; i++)
         {
            const float x = glm::mix(lowX, highX, static_cast<float>(i) / samples);
            points.push_back({x, railHeight(x), railDepth(x)});
         }
         marimba.frame.append(extrudeAlong(railProfile, points));
         if (fraction > 0.5f)
         {
            endZ[row][0] = points.front().z;
            endZ[row][1] = points.back().z;
         }
         railY[row] = points.front().y;
      }
   }

   //  turned legs front and back, a beam across, and a stretcher between the ends
   const float stretcherY = 0.22f;
   glm::vec3 stretcherEnds[2];
   for (int end = 0; end < 2; end++)
   {
      const float x = end == 0 ? lowX : highX;
      const float front = endZ[0][end];
      const float back = endZ[1][end];
      //  legs stop under the rails; the beam slopes up to the raised accidental row
      marimba.frame.append(legMesh(railY[0] - 0.02f), glm::translate(glm::mat4(1), glm::vec3(x, 0, front)));
      marimba.frame.append(legMesh(railY[1] - 0.02f), glm::translate(glm::mat4(1), glm::vec3(x, 0, back)));
      marimba.frame.append(extrudeBetween(beamProfile, {x, railY[0], front + 0.02f}, {x, railY[1], back - 0.02f}));
      marimba.frame.append(extrudeBetween(rodProfile, {x, stretcherY, front}, {x, stretcherY, back}));
      stretcherEnds[end] = {x, stretcherY, 0.5f * (front + back)};
   }
   marimba.frame.append(extrudeBetween(rodProfile, stretcherEnds[0], stretcherEnds[1]));

   return marimba;
}
