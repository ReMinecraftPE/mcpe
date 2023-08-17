# ReMinecraftPE - Makefile for linux

# Commonly included directories.
SRC_DIR=source
BLD_DIR=build
RKN_DIR=thirdparty/raknet
ZLB_DIR=thirdparty/zlib
PLT_DIR=platforms

# Target executable's name.
TARGET=minecraftcpp

# Compilation flags for C++ source files
CXXFLAGS=-Isource -I. -Ithirdparty/raknet -Ithirdparty/zlib -DUSE_SDL -DUSE_OPENAL -DUSE_MATH_DEFINES -DHANDLE_CHARS_SEPARATELY -O3 -MMD

GLFLAGS=\
	-DxglVertexPointer=glVertexPointer \
	-DxglEnableClientState=glEnableClientState \
	-DxglTexCoordPointer=glTexCoordPointer \
	-DxglColorPointer=glColorPointer \
	-DxglDrawArrays=glDrawArrays \
	-DxglDisableClientState=glDisableClientState

# Compilation flags for zlib source files
ZLIBFLAGS=-O3 -I. -MMD

# Link flags
LINKFLAGS=-L/opt/vc/lib/ -lpng -lpthread -lSDL2 -lGL -lopenal -lGLU

#include everything in source/, plus certain files from platforms
SRC_FILES = $(shell find $(SRC_DIR)                       -type f -name '*.cpp')
PLT_FILES = $(shell find $(PLT_DIR)/sdl $(PLT_DIR)/openal -type f -name '*.cpp')
RKN_FILES = $(shell find $(RKN_DIR)                       -type f -name '*.cpp')
ZLB_FILES = $(shell find $(ZLB_DIR)                       -type f -name '*.c')

OBJ_FILES = \
	$(patsubst $(SRC_DIR)/%,$(BLD_DIR)/s/%,$(SRC_FILES:.cpp=.o)) \
	$(patsubst $(PLT_DIR)/%,$(BLD_DIR)/p/%,$(PLT_FILES:.cpp=.o)) \
	$(patsubst $(RKN_DIR)/%,$(BLD_DIR)/r/%,$(RKN_FILES:.cpp=.o)) \
	$(patsubst $(ZLB_DIR)/%,$(BLD_DIR)/z/%,$(ZLB_FILES:.c=.o))
	
DEP_FILES = \
	$(patsubst $(SRC_DIR)/%,$(BLD_DIR)/s/%,$(SRC_FILES:.cpp=.d)) \
	$(patsubst $(PLT_DIR)/%,$(BLD_DIR)/p/%,$(PLT_FILES:.cpp=.d)) \
	$(patsubst $(RKN_DIR)/%,$(BLD_DIR)/r/%,$(RKN_FILES:.cpp=.d)) \
	$(patsubst $(ZLB_DIR)/%,$(BLD_DIR)/z/%,$(ZLB_FILES:.c=.d))

#default target.
.PHONY = all
all: program

#link rules for the executable
$(TARGET): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LINKFLAGS)

#include header dependencies
-include $(DEP_FILES)

$(BLD_DIR)/p/%.o: $(PLT_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GLFLAGS) -c -o $@ $<

$(BLD_DIR)/r/%.o: $(RKN_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BLD_DIR)/z/%.o: $(ZLB_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(ZLIBFLAGS) -c -o $@ $<

$(BLD_DIR)/s/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GLFLAGS) -c -o $@ $<

program:  $(TARGET)

clean:
	rm -rf $(BLD_DIR)
	rm -rf minecraftcpp
