BINARY = LoD
SRC_DIR = src
OBJ_DIR = .obj

CPP_FILES := $(shell find -L $(SRC_DIR) -name '*.cc')
OBJECTS := $(subst $(SRC_DIR),$(OBJ_DIR),$(CPP_FILES:.cc=.o))
DEPS := $(OBJECTS:.o=.dep)

CXX = clang++
CXXFLAGS = -g -rdynamic -std=c++11 -Wall -Qunused-arguments \
					 `pkg-config --cflags glfw3` `Magick++-config --cxxflags --cppflags`
LDFLAGS = -g -rdynamic -lGL -lGLU -lGLEW -lassimp `pkg-config --libs glfw3` \
					-lXxf86vm -lX11 -lXrandr -lXi -lm -lXcursor -lpthread \
					`Magick++-config --ldflags --libs` -lfreetype \
					-Lsrc/engine/gui/freetype-gl -lfreetype-gl

.PHONY: all clean

all: $(BINARY)

clean:
	rm -f $(BINARY) -rf $(OBJ_DIR)

$(OBJ_DIR)/%.dep: $(SRC_DIR)/%.cc
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.dep=.cc)) -MT $(@:.dep=.o) -MF $@

-include $(DEPS)

$(OBJ_DIR)/%.o:
	$(CXX) $(CXXFLAGS) -c $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.o=.cc)) -o $@

$(BINARY): $(DEPS) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
