# Copyright (c) 2014, Tamas Csala

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

NORMAL = \e[0m
GREEN = \e[32m
RED = \e[91m
BOLD = \e[1m
YELLOW = \e[93m

ifneq ($(MAKECMDGOALS),nocolor)
	printf = @/bin/echo -e "$(2)$(1)$(NORMAL)"
else
	printf = $(info $(1))
endif

.PHONY: all nocolor clean

all: $(BINARY)
nocolor: $(BINARY)

clean:
	@rm -f $(BINARY) -rf $(OBJ_DIR)

$(OBJ_DIR)/%.dep: $(SRC_DIR)/%.cc
	$(call printf,Creating CXX dependency $@,$(YELLOW))
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.dep=.cc)) -MT $(@:.dep=.o) -MF $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

$(OBJ_DIR)/%.o:
	$(call printf,Building CXX object $@,$(GREEN))
	@$(CXX) $(CXXFLAGS) -c $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.o=.cc)) -o $@

$(BINARY): $(DEPS) $(OBJECTS)
	$(call printf,Linking CXX executable $@,$(BOLD)$(RED))
	@$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Workaround for dependency change
%.cc %.h:
