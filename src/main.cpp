//  Procedural MIDI instrument stage
//  Entry point: SDL2 window with an OpenGL 3.3 core context
#include "camera.h"
#include "gl/mesh.h"
#include "gl/shader.h"
#include "gl/text.h"
#include "instruments/marimba.h"
#include "render/light.h"
#include "render/material.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>

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
   //  4x multisample anti-aliasing for smooth edges
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

   const Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
   SDL_Window *window = SDL_CreateWindow("autopolyphonic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, flags);
   //  some machines have no multisampled visual; fall back to none rather than failing
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

      Mesh floor(makePlane(6.0f));

      //  each bar gets its own mesh so it can move and glow (?) on its own later
      const Marimba marimba = buildMarimba();
      std::vector<Mesh> barMeshes;
      for (const Marimba::Bar &bar : marimba.bars)
         barMeshes.emplace_back(bar.mesh);
      Mesh resonators(marimba.resonators);
      Mesh frame(marimba.frame);
      Camera camera;
      Text text;
      bool wireframe = false;
      bool normalView = false;
      const DirectionalLight light{{0.5f, 1.0f, 0.6f}, {1.0f, 1.0f, 0.9f}, {0.04f, 0.04f, 0.05f}};

      auto draw = [&phong](const Mesh &mesh, const glm::mat4 &model, const Material &material)
      {
         phong.set("uModel", model);
         phong.set("uNormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
         material.apply(phong);
         mesh.draw();
      };

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
                  camera.dim += 0.1f;
                  break;
               case SDLK_PAGEUP:
                  camera.dim = std::max(camera.dim - 0.1f, 1.0f);
                  break;
               case SDLK_0:
                  camera.th = camera.ph = 0;
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
            else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK))
            {
               camera.th += 0.3f * event.motion.xrel;
               camera.ph += 0.3f * event.motion.yrel;
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

         draw(floor, glm::mat4(1), materials::floor);
         for (size_t i = 0; i < marimba.bars.size(); i++)
            draw(barMeshes[i], glm::translate(glm::mat4(1), marimba.bars[i].center), materials::rosewood);
         draw(resonators, glm::mat4(1), materials::brass);
         draw(frame, glm::mat4(1), materials::darkWood);

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
