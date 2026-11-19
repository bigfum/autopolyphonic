//  Procedural MIDI instrument stage
//  Entry point: SDL2 window with an OpenGL 3.3 core context
#include "camera.h"
#include "gl/shader.h"
#include "gl/text.h"
#include "instruments/cymbal.h"
#include "instruments/drum.h"
#include "instruments/marimba.h"
#include "instruments/stand.h"
#include "render/light.h"
#include "render/stage.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

static void fatal(const char *msg)
{
   std::fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
   std::exit(1);
}

int main(int, char **)
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
      fatal("SDL_Init failed");

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

   const Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
   SDL_Window *window = SDL_CreateWindow("autopolyphonic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, flags);
   if (!window)
   {
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
      window = SDL_CreateWindow("autopolyphonic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, flags);
   }
   if (!window)
      fatal("SDL_CreateWindow failed");

   SDL_GLContext context = SDL_GL_CreateContext(window);
   if (!context)
      fatal("SDL_GL_CreateContext failed");
   SDL_GL_SetSwapInterval(1);

   if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
      fatal("gladLoadGLLoader failed");
   std::printf("OpenGL %s\n", glGetString(GL_VERSION));

   {
      Shader phong("shaders/phong.vert", "shaders/phong.frag");

      const float deckHeight = 0.25f;
      Model floor;
      floor.add(makePlane(30.0f), materials::floor);
      Stage stage;
      stage.add(floor);
      stage.add(buildDeck(7.0f, 4.5f, deckHeight));
      stage.add(buildMarimba(), glm::translate(glm::mat4(1), glm::vec3(0, deckHeight, 1.0f)));
      const DrumSpec tom{0.4f, 0.4f, 8};
      const float tomHeight = 0.6f;
      const glm::mat4 tomHead = glm::translate(glm::mat4(1), glm::vec3(-2.3f, deckHeight + tomHeight, 0.0f));
      stage.add(buildDrum(tom, materials::lacquer, 41), tomHead);
      stage.add(buildFloorDrumLegs(tom.diameter, tomHeight), tomHead);
      const DrumSpec snare{0.35f, 0.15f, 10};
      const float snareHeight = 0.65f;
      const glm::mat4 snareHead = glm::translate(glm::mat4(1), glm::vec3(-1.6f, deckHeight + snareHeight, -0.6f));
      const float snareTilt = 10.0f;
      stage.add(buildDrum(snare, materials::lacquer, 38), snareHead * drumStandTilt(snare, snareTilt));
      stage.add(buildDrumStand(snare, snareHeight, snareTilt), snareHead);
      const StandSpec crashStand{1.2f, 3, 0.3f, 15.0f};
      const glm::mat4 crashBase = glm::translate(glm::mat4(1), glm::vec3(-2.8f, deckHeight, -0.6f));
      stage.add(buildCymbalStand(crashStand), crashBase);
      stage.add(buildCymbal(0.45f, 49),
                glm::rotate(glm::translate(crashBase, glm::vec3(0, crashStand.height, 0)), glm::radians(crashStand.tilt),
                            glm::vec3(1, 0, 0)));
      Camera camera;
      Text text;
      bool wireframe = false;
      bool normalView = false;
      const DirectionalLight light{{0.5f, 1.0f, 0.6f}, {1.0f, 1.0f, 0.9f}, {0.04f, 0.04f, 0.05f}};

      glEnable(GL_CULL_FACE);
      glEnable(GL_MULTISAMPLE);

      bool running = true;
      while (running)
      {
         SDL_Event event;
         while (SDL_PollEvent(&event))
         {
            if (event.type == SDL_QUIT)
               running = false;
            else if (event.type == SDL_KEYDOWN)
            {
               switch (event.key.keysym.sym)
               {
               case SDLK_ESCAPE:
                  running = false;
                  break;
               case SDLK_RIGHT:
                  camera.th += 5;
                  break;
               case SDLK_LEFT:
                  camera.th -= 5;
                  break;
               case SDLK_UP:
                  camera.ph += 5;
                  break;
               case SDLK_DOWN:
                  camera.ph -= 5;
                  break;
               case SDLK_PAGEDOWN:
                  camera.zoom(1.1f);
                  break;
               case SDLK_PAGEUP:
                  camera.zoom(1 / 1.1f);
                  break;
               case SDLK_0:
                  camera.reset();
                  break;
               case SDLK_EQUALS:
                  camera.fov = std::min(camera.fov + 1, 179.0f);
                  break;
               case SDLK_MINUS:
                  camera.fov = std::max(camera.fov - 1, 1.0f);
                  break;
               case SDLK_w:
                  wireframe = !wireframe;
                  break;
               case SDLK_n:
                  normalView = !normalView;
                  break;
               }
            }
            else if (event.type == SDL_MOUSEMOTION &&
                     (event.motion.state & (SDL_BUTTON_LMASK | SDL_BUTTON_MMASK | SDL_BUTTON_RMASK)))
            {
               const SDL_Keymod mod = SDL_GetModState();
               if ((mod & KMOD_SHIFT) || (event.motion.state & SDL_BUTTON_RMASK))
               {
                  int windowHeight;
                  SDL_GetWindowSize(window, nullptr, &windowHeight);
                  camera.pan(event.motion.xrel, event.motion.yrel, windowHeight);
               }
               else if (mod & KMOD_CTRL)
                  camera.zoom(std::pow(1.01f, event.motion.yrel));
               else
                  camera.orbit(event.motion.xrel, event.motion.yrel);
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
#if SDL_VERSION_ATLEAST(2, 0, 18)
               const float amount = event.wheel.preciseY;
#else
               const float amount = event.wheel.y;
#endif
               camera.zoom(std::pow(0.9f, amount));
            }
         }
         camera.ph = std::clamp(camera.ph, -85.0f, 85.0f);

         int w, h;
         SDL_GL_GetDrawableSize(window, &w, &h);
         glViewport(0, 0, w, h);
         int windowWidth;
         SDL_GetWindowSize(window, &windowWidth, nullptr);
         text.resize(windowWidth, w, h);

         glClearColor(0.01f, 0.01f, 0.015f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glEnable(GL_DEPTH_TEST);
         glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

         phong.use();
         phong.set("uView", camera.view());
         phong.set("uProjection", camera.projection(static_cast<float>(w) / h));
         phong.set("uEye", camera.eye());
         phong.set("uNormalView", normalView ? 1 : 0);
         light.apply(phong);

         stage.draw(phong);

         text.print(5, 5, "Angle=%.0f,%.0f  Dim=%.1f  FOV=%.0f  Wireframe=%s  Normals=%s", camera.th, camera.ph,
                    camera.dim, camera.fov, wireframe ? "On" : "Off", normalView ? "On" : "Off");

         SDL_GL_SwapWindow(window);
      }
   }

   SDL_GL_DeleteContext(context);
   SDL_DestroyWindow(window);
   SDL_Quit();
   return 0;
}
