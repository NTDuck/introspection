# compiler & flags
cxx = g++
cxxflags = -Wall -I"include" -I"include/sdl2" -I"include/headers"

# libs
libpath = -L/usr/lib
libs = -lSDL2 -lSDL2_image -lSDL2_ttf -lz

# src & output
src = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)
output = build/8964

all: $(output)

$(output): $(src)
	$(cxx) $(cxxflags) $(libpath) -o $@ $^ $(libs)

clean:
	rm -f $(output)

# make
# ./build/8964
# gdb ./build/8964   # Beware of segfault!