//  Procedural MIDI instrument stage
//  Entry point: SDL2 window with an OpenGL 3.3 core context
#include "camera.h"
#include "gl/shader.h"
#include "gl/text.h"
#include "instruments/drumkit.h"
#include "instruments/marimba.h"
#include "render/light.h"
#include "render/shadow.h"
#include "render/stage.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
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
      addDrumKit(stage, glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-2.3f, deckHeight, -0.5f)),
                                    glm::pi<float>(), glm::vec3(0, 1, 0)));
      Camera camera;
      Text text;
      bool wireframe = false;
      bool normalView = false;
      bool showShadowMap = false;
      ShadowMap shadow;
      const DirectionalLight light{{0.5f, 1.0f, 0.6f}, {1.0f, 1.0f, 0.9f}, {0.04f, 0.04f, 0.05f}};

      glEnable(GL_CULL_FACE);
      glEnable(GL_MULTISAMPLE);

      //  demo until MIDI (ride on every beat, marimba bars up the keyboard on eighth notes)
      const float eighth = 0.25f;
      int lastEighth = -1;

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
               case SDLK_m:
                  showShadowMap = !showShadowMap;
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

         const float now = SDL_GetTicks() / 1000.0f;
         const int count = static_cast<int>(now / eighth);
         if (count != lastEighth)
         {
            lastEighth = count;
            const float hit = count * eighth;
            if (count % 2 == 0)
               stage.strike(9, 51, hit);
            if (count % 4 == 0)
               stage.strike(9, 36, hit);
            if (count % 4 == 2)
               stage.strike(9, 38, hit);
            stage.strike(0, 45 + count % 52, hit);
         }

         glEnable(GL_DEPTH_TEST);
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
         stage.draw(shadow.begin(light.direction, glm::vec3(0, 1, 0), 4.5f), now);
         shadow.end();

         int w, h;
         SDL_GL_GetDrawableSize(window, &w, &h);
         glViewport(0, 0, w, h);
         int windowWidth;
         SDL_GetWindowSize(window, &windowWidth, nullptr);
         text.resize(windowWidth, w, h);

         glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

         phong.use();
         phong.set("uView", camera.view());
         phong.set("uProjection", camera.projection(static_cast<float>(w) / h));
         phong.set("uEye", camera.eye());
         phong.set("uNormalView", normalView ? 1 : 0);
         light.apply(phong);
         shadow.apply(phong, 1);

         stage.draw(phong, now);
         if (showShadowMap)
            shadow.drawDebug(w, h);

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
