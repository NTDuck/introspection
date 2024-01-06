cxx = g++
cxxflags = -Wall -I"include" -I"include/sdl2" -I"include/headers"

src = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)

# Detect OS
ifeq ($(OS),Windows_NT)   # OS is a preexisting environment variable on Windows
	OS = windows
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		OS = macos
	else ifeq ($(UNAME),Linux)
		OS = linux
	else
    	$(error Unsupported OS)
	endif
endif

ifeq ($(OS),windows)
	libpath = -L"lib"
	libs = -l"mingw32" -l"SDL2main" -l"SDL2" -l"SDL2_image" -l"SDL2_ttf" -l"z"
	output = build/8964.exe
else ifeq ($(OS),linux)
	libpath = -L/usr/lib
	libs = -lSDL2 -lSDL2_image -lSDL2_ttf -lz
	output = build/8964
endif

all: $(output)

$(output): $(src)
	$(cxx) $(cxxflags) $(libpath) -o $@ $^ $(libs)

clean:
	ifeq ($(OS),windows)
		del $(output)
	else ifeq ($(OS),linux)
		rm -f $(output)
	endif

# https://stackoverflow.com/questions/64396979/how-do-i-use-sdl2-in-my-programs-correctly