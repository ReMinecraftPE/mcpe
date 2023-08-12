# ReMinecraftPE - Makefile for linux

SRC_DIR=source
BLD_DIR=build
RAKNET_DIR=thirdparty/raknet
PLATFORM_DIR=platforms
TARGET=minecraftcpp

CXXFLAGS=-Isource -I. -Ithirdparty/raknet -DUSE_SDL -DUSE_OPENAL -DUSE_MATH_DEFINES -DHANDLE_CHARS_SEPARATELY -O3 -MMD
LINKFLAGS=-L/opt/vc/lib/ -lpng -lpthread -lSDL2 -lGL -lopenal -lGLU

#include everything in source/, plus certain files from platforms
SRC_FILES = $(shell find $(SRC_DIR)      -type f -name '*.cpp')
PLT_FILES = $(shell find platforms/sdl platforms/openal -type f -name '*.cpp')
RKN_FILES = $(shell find $(RAKNET_DIR)   -type f -name '*.cpp')
OBJ_FILES = $(patsubst $(SRC_DIR)/%,$(BLD_DIR)/s/%,$(SRC_FILES:.cpp=.o)) $(patsubst $(PLATFORM_DIR)/%,$(BLD_DIR)/p/%,$(PLT_FILES:.cpp=.o)) $(patsubst $(PLATFORM_DIR)/%,$(BLD_DIR)/r/%,$(RKN_FILES:.cpp=.o))
DEP_FILES = $(patsubst $(SRC_DIR)/%,$(BLD_DIR)/s/%,$(SRC_FILES:.cpp=.d)) $(patsubst $(PLATFORM_DIR)/%,$(BLD_DIR)/p/%,$(PLT_FILES:.cpp=.d)) $(patsubst $(PLATFORM_DIR)/%,$(BLD_DIR)/r/%,$(RKN_FILES:.cpp=.d))

#default target.
.PHONY = all
all: program

#link rules for the executable
$(TARGET): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LINKFLAGS)

#include header dependencies
-include $(DEP_FILES)

$(BLD_DIR)/s/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BLD_DIR)/p/%.o: $(PLATFORM_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BLD_DIR)/r/%.o: $(RAKNET_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
	
program:  $(TARGET)


clean:
	rm -rf $(BLD_DIR)
	rm -rf minecraftcpp
