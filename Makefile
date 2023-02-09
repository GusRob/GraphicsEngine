PROJECT_NAME := main

BUILD_DIR := build

# Define the names of key files
SOURCE_FILE := src/$(PROJECT_NAME).cpp
OBJECT_FILE := $(BUILD_DIR)/$(PROJECT_NAME).o
EXECUTABLE := $(BUILD_DIR)/$(PROJECT_NAME)
SDW_DIR := ./libs/sdw/
REND_DIR := ./libs/rend/
SDW_SOURCE_FILES := $(wildcard $(SDW_DIR)*.cpp)
SDW_OBJECT_FILES := $(patsubst $(SDW_DIR)%.cpp, $(BUILD_DIR)/%.o, $(SDW_SOURCE_FILES))
REND_SOURCE_FILES := $(wildcard $(REND_DIR)*.cpp)
REND_OBJECT_FILES := $(patsubst $(REND_DIR)%.cpp, $(BUILD_DIR)/%.o, $(REND_SOURCE_FILES))

# Build settings
COMPILER := clang++
COMPILER_OPTIONS := -c -pipe -Wall -std=c++11 # If you have an older compiler, you might have to use -std=c++0x
DEBUG_OPTIONS := -ggdb -g3
FUSSY_OPTIONS := -Werror -pedantic
SANITIZER_OPTIONS := -O1 -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer
SPEEDY_OPTIONS := -Ofast -funsafe-math-optimizations -march=native
LINKER_OPTIONS :=

# Set up flags
SDW_COMPILER_FLAGS := -I$(SDW_DIR)
# If you have a manual install of SDL, you might not have sdl2-config installed, so the following line might not work
# Compiler flags should look something like: -I/usr/local/include/SDL2 -D_THREAD_SAFE
SDL_COMPILER_FLAGS := $(shell sdl2-config --cflags)
# If you have a manual install of SDL, you might not have sdl2-config installed, so the following line might not work
# Linker flags should look something like: -L/usr/local/lib -lSDL2
SDL_LINKER_FLAGS := -lSDL2_image -lSDL2_ttf $(shell sdl2-config --libs)
SDW_LINKER_FLAGS := $(SDW_OBJECT_FILES)

REND_COMPILER_FLAGS := -I$(REND_DIR)
REND_LINKER_FLAGS := $(REND_OBJECT_FILES)

default: debug

# Rule to compile and link for use with a debugger (although works fine even if you aren't using a debugger !)
debug: $(SDW_OBJECT_FILES) $(REND_OBJECT_FILES)
	$(COMPILER) $(COMPILER_OPTIONS) $(DEBUG_OPTIONS) -o $(OBJECT_FILE) $(SOURCE_FILE) $(SDL_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS) $(REND_COMPILER_FLAGS)
	$(COMPILER) $(LINKER_OPTIONS) $(DEBUG_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILE) $(REND_LINKER_FLAGS) $(SDW_LINKER_FLAGS) $(SDL_LINKER_FLAGS)
	./$(EXECUTABLE)

# Rule to build for high performance executable (for manually testing interaction)
speedy: $(SDW_OBJECT_FILES)
	$(COMPILER) $(COMPILER_OPTIONS) $(SPEEDY_OPTIONS) -o $(OBJECT_FILE) $(SOURCE_FILE) $(SDL_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS) $(REND_COMPILER_FLAGS)
	$(COMPILER) $(LINKER_OPTIONS) $(SPEEDY_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILE) $(REND_LINKER_FLAGS) $(SDW_LINKER_FLAGS) $(SDL_LINKER_FLAGS)
	./$(EXECUTABLE)

# Rule for building all of the the DisplayWindow classes
$(BUILD_DIR)/%.o: $(SDW_DIR)%.cpp
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(COMPILER_OPTIONS) -c -o $@ $^ $(SDL_COMPILER_FLAGS)

# Rule for building all of the the rendering classes
$(BUILD_DIR)/%.o: $(REND_DIR)%.cpp
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(COMPILER_OPTIONS) -c -o $@ $^ $(SDL_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS)

# Files to remove during clean
clean:
	rm $(BUILD_DIR)/*