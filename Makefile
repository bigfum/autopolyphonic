# autopolyphonic
EXE=autopolyphonic

# Main target
all: $(EXE)

SRC=src/main.cpp src/camera.cpp src/gl/shader.cpp src/gl/mesh.cpp src/geometry/shapes.cpp
OBJ=$(SRC:.cpp=.o) third_party/glad/src/glad.o
INC=-Isrc -Ithird_party/glad/include -Ithird_party/glm

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
LIBS=-lmingw32 -lSDL2main -lSDL2 -mwindows -lopengl32 -lm
CLEAN=rm -f $(EXE).exe
else
#  OSX (Homebrew paths are needed on Apple Silicon)
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -I/opt/homebrew/include
LIBS=-L/opt/homebrew/lib -lSDL2main -lSDL2 -framework Cocoa -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lSDL2 -lGL -lm
endif
CLEAN=rm -f $(EXE)
endif

# Compile rules (-MMD -MP track header dependencies in .d files)
%.o: %.c
	gcc -c $(CFLG) $(INC) -MMD -MP -o $@ $<
%.o: %.cpp
	g++ -c -std=c++17 $(CFLG) -Wextra $(INC) -MMD -MP -o $@ $<

#  Link
$(EXE): $(OBJ)
	g++ $(CFLG) -o $@ $^ $(LIBS)

#  Build and run (shaders load relative to the project folder)
run: $(EXE)
	./$(EXE)

#  Clean
clean:
	$(CLEAN) $(OBJ) $(OBJ:.o=.d)

-include $(OBJ:.o=.d)
.PHONY: all run clean
