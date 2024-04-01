################################################################################
#### General configurations
################################################################################

# Executable name
EXEC = 8964

# Build and assets directories (same directory for all platforms => bad practice)
BUILD_DIR = build
ASSETS_DIR = assets

# Sources
SRC_DIR = src
SRCS := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp) $(wildcard $(SRC_DIR)/*/*/*.cpp)   # Recursively search inside source directory within 3 first levels. Verbose syntax since some platforms might not support `$(sort $(shell find $(SRC_DIR) -name '*.cpp'))`

# Includes
INCLUDE_DIR = include
INCLUDES := -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/sdl2 -I$(INCLUDE_DIR)/headers

# C preprocessor settings
CPPFLAGS = $(INCLUDES) -MMD -MP

# C++ compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -fprofile-generate
# Removed: -flto (causing compilation error `multiple definition of `construction vtable for Multiton<Teleporter>-in-Teleporter'`)
WARNINGS = -Wall

# Linker flags
LDFLAGS =

# Libraries to link
LDLIBS = -l"SDL2" -l"SDL2_image" -l"SDL2_ttf" -l"SDL2_mixer"

# Debug (default) and release modes settings
# https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
ifeq ($(release),1)
	# BUILD_DIR := $(BUILD_DIR)/release
	# BIN_DIR := $(BIN_DIR)/release
	CXXFLAGS += -O3
	CPPFLAGS += -DNDEBUG
else
	# BUILD_DIR := $(BUILD_DIR)/debug
	# BIN_DIR := $(BIN_DIR)/debug
	CXXFLAGS += -Ofast -march=native -mfpmath=sse -freorder-blocks -fpredictive-commoning -fno-threadsafe-statics -ffloat-store -ffast-math -fno-rounding-math -fno-signaling-nans -fcx-limited-range -fno-math-errno -funsafe-math-optimizations -fassociative-math -freciprocal-math -ffinite-math-only -fno-signed-zeros -fno-trapping-math -frounding-math -fsingle-precision-constant -fcx-fortran-rules
endif

# Target OS detection
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

################################################################################
#### Platform-specific configurations
################################################################################

ifeq ($(OS),windows)   # Windows 32-bit and 64-bit common settings
	INCLUDES +=
	LDFLAGS += -static-libgcc -static-libstdc++
	LDLIBS := -l"mingw32" -l"SDL2main" $(LDLIBS) -l"zlib"

	LIB_PATH = -L"lib"
	EXEC := $(EXEC).exe
	RM = del /Q

	ifeq ($(win32),1)
		# Windows 32-bit settings
		INCLUDES +=
		LDFLAGS +=
		LDLIBS +=

		CXXFLAGS += -m32
	else
		# Windows 64-bit settings
		INCLUDES +=
		LDFLAGS +=
		LDLIBS +=

		CXXFLAGS += -m64
	endif

else ifeq ($(OS),macos)   # macOS-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS += -l"z"

else ifeq ($(OS),linux)   # Linux-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS += -l"z"

	LIB_PATH = -L/usr/lib
	RM = rm -f
endif

OUTPUT = $(BUILD_DIR)/$(EXEC)

################################################################################
#### Burenyuu~
################################################################################

.PHONY: all
all: $(OUTPUT)

# Should this be compartmentalized?
$(OUTPUT): $(SRCS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(WARNINGS) $(LIB_PATH) -o $@ $^ $(LDLIBS)

.PHONY: run
run:
	./$(OUTPUT)

.PHONY: rund
rund:
	gdb -ex run ./$(OUTPUT)

.PHONY: clean
clean:
	@echo Cleaning $(BUILD_DIR) directory
	$(RM) $(OUTPUT)

# https://stackoverflow.com/questions/64396979/how-do-i-use-sdl2-in-my-programs-correctly