//  Procedural MIDI instrument stage
//  Entry point: SDL2 window with an OpenGL 3.3 core context
#include "camera.h"
#include "gl/mesh.h"
#include "geometry/extrude.h"
#include "geometry/lathe.h"
#include "geometry/profile.h"
#include "gl/shader.h"
#include "gl/text.h"
#include "render/light.h"
#include "render/material.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
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

   SDL_Window *window = SDL_CreateWindow("autopolyphonic",
                                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
                                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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

      Mesh floor(makePlane(4.0f));
      Mesh ball(makeSphere(0.6f, 24, 48));

      //  temp lathe demo until the marimba replaces it.

      //  tube
      const float r = 0.08f, wall = 0.005f, length = 0.9f;
      Mesh tube(lathe({{0, 0}, {r, 0}, {r, length}, {r - wall, length}, {r - wall, wall}, {0, wall}}, 32));

      //  turned leg, smooth bits and creased bits
      Mesh leg(lathe({{0, 0}, {0.03f, 0}, {0.032f, 0.015f}, {0.026f, 0.035f}, {0.019f, 0.07f}, {0.017f, 0.8f}, {0.024f, 0.825f}, {0.024f, 0.845f}, {0.02f, 0.86f}, {0.02f, 0.9f}, {0, 0.9f}},
                     24));

      //  temp extrusion demo: side view of a bar (length along x), counter-clockwise, with arched undercut
      const float barLength = 0.8f, barThick = 0.03f, barWidth = 0.06f, node = 0.224f;
      std::vector<glm::vec2> barOutline = {{-0.5f * barLength, -0.5f * barThick}};
      const float archHalf = (0.5f - node) * barLength;
      for (int k = 0; k <= 16; k++)
      {
         const float x = -archHalf + 2 * archHalf * k / 16;
         const float lift = 0.5f * (1 + std::cos(3.14159265f * x / archHalf));
         barOutline.push_back({x, -0.5f * barThick + 0.4f * barThick * lift});
      }
      barOutline.insert(barOutline.end(), {{0.5f * barLength, -0.5f * barThick},
                                           {0.5f * barLength, 0.5f * barThick},
                                           {-0.5f * barLength, 0.5f * barThick}});
      Mesh bar(extrude(barOutline, barWidth));
      //  two rails under the bar's nodes
      const std::vector<glm::vec2> railOutline = roundedRect(0.022f, 0.04f, 0.005f);
      const float railX = (0.5f - node) * barLength;
      Mesh railA(extrudeBetween(railOutline, {-railX, 0.5f, -0.25f}, {-railX, 0.5f, 0.25f}));
      Mesh railB(extrudeBetween(railOutline, {railX, 0.5f, -0.25f}, {railX, 0.5f, 0.25f}));
      Camera camera;
      Text text;
      bool wireframe = false;
      const DirectionalLight light{{0.5f, 1.0f, 0.6f}, {1.0f, 1.0f, 0.9f}, {0.04f, 0.04f, 0.05f}};

      auto draw = [&phong](const Mesh &mesh, const glm::mat4 &model, const Material &material)
      {
         phong.set("uModel", model);
         phong.set("uNormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
         material.apply(phong);
         mesh.draw();
      };

      glEnable(GL_CULL_FACE);

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
         light.apply(phong);

         draw(floor, glm::mat4(1), materials::floor);
         draw(ball, glm::translate(glm::mat4(1), glm::vec3(0, 0.6f, -1.2f)), materials::plastic);
         draw(bar, glm::translate(glm::mat4(1), glm::vec3(0, 0.543f, -0.5f * barWidth)), materials::rosewood);
         draw(railA, glm::mat4(1), materials::darkWood);
         draw(railB, glm::mat4(1), materials::darkWood);
         draw(tube, glm::translate(glm::mat4(1), glm::vec3(1.1f, 0, 0.3f)), materials::brass);
         draw(leg, glm::scale(glm::translate(glm::mat4(1), glm::vec3(-1.1f, 0, 0.3f)), glm::vec3(1.5f, 1, 1.5f)), materials::darkWood);

         text.print(5, 5, "Angle=%.0f,%.0f  Dim=%.1f  FOV=%.0f  Wireframe=%s", camera.th, camera.ph, camera.dim,
                    camera.fov, wireframe ? "On" : "Off");

         SDL_GL_SwapWindow(window);
      }
   }

   SDL_GL_DeleteContext(context);
   SDL_DestroyWindow(window);
   SDL_Quit();
   return 0;
}
