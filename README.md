# autopolyphonic
procedurally-built 3D instruments that perform MIDI files, built with C++, SDL2, and OpenGL

## Build

Requires CMake and SDL2 (macOS: `brew install cmake sdl2`).

```sh
cmake -B build
cmake --build build
./build/autopolyphonic
```

## Credits / licenses

- [glad](https://github.com/Dav1dde/glad) 0.1.36 generated loader (OpenGL 3.3 core), MIT; `khrplatform.h` is © The Khronos Group, MIT-style license (see header).
