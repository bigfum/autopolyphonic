# autopolyphonic
procedurally-built 3D instruments that perform MIDI files, built with C++, SDL2, and OpenGL

## Build

Requires SDL2 and a C++17 compiler; glad and glm are included in `third_party/`.

- Linux: `sudo apt install g++ make libsdl2-dev libgl-dev`
- macOS: `brew install sdl2`
- Windows: MSYS2/MinGW with the `SDL2` package

```sh
make
./autopolyphonic
```

Run it from the project folder so it can find `shaders/`. `make run` does both steps.

## Credits / licenses

- [glad](https://github.com/Dav1dde/glad) 0.1.36 generated loader (OpenGL 3.3 core), MIT; `khrplatform.h` is © The Khronos Group, MIT-style license (see header).
- [glm](https://github.com/g-truc/glm) 1.0.3 math library, MIT license (see `third_party/glm/copying.txt`).
