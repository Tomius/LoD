# Copyright (c) 2014, Tamas Csala

BINARY = LoD
SRC_DIR = src
OBJ_DIR = .obj

CPP_FILES := $(shell find -L $(SRC_DIR) -name '*.cc')
OBJECTS := $(subst $(SRC_DIR),$(OBJ_DIR),$(CPP_FILES:.cc=.o))
DEPS := $(OBJECTS:.o=.d)

CXX = clang++

BASE_CXXFLAGS = -std=c++11 -Wall -Qunused-arguments \
					 			`pkg-config --cflags glfw3` `Magick++-config --cxxflags --cppflags`

ifneq ($(MAKECMDGOALS),release)
	CXXFLAGS = -g -rdynamic $(BASE_CXXFLAGS)
else
	CXXFLAGS = -O3 -DOGLWRAP_DEBUG=0 $(BASE_CXXFLAGS)
endif

BASE_LDFLAGS =  -lGL -lGLU -lGLEW -lassimp `pkg-config --libs glfw3` \
								-lXxf86vm -lX11 -lXrandr -lXi -lm -lXcursor -lpthread \
								`Magick++-config --ldflags --libs` -lfreetype \
								-Lsrc/engine/gui/freetype-gl -lfreetype-gl

ifneq ($(MAKECMDGOALS),release)
	LDFLAGS = -g -rdynamic $(BASE_LDFLAGS)
else
	LDFLAGS = $(BASE_LDFLAGS)
endif

NORMAL = \e[0m
GREEN = \e[32m
RED = \e[91m
BOLD = \e[1m
YELLOW = \e[93m

ifneq ($(MAKECMDGOALS),nocolor)
	printf = @/bin/echo -e "$(2)$(subst $(OBJ_DIR)/,,$(1))$(NORMAL)"
else
	printf = $(info $(subst $(OBJ_DIR)/,,$(1)))
endif

.PHONY: all clean nocolor release

all: $(BINARY)
nocolor: $(BINARY)
release: $(BINARY)

clean:
	@rm -f $(BINARY) -rf $(OBJ_DIR)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cc
	$(call printf,Creating CXX dependency list $@,$(YELLOW))
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.d=.cc)) -MT $(@:.d=.o) -MF $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.d
	$(call printf,Building CXX object $@,$(GREEN))
	@$(CXX) $(CXXFLAGS) -c $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.o=.cc)) -o $@

$(BINARY): $(OBJECTS)
	$(call printf,Linking CXX executable $@,$(BOLD)$(RED))
	@$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Workaround for dependency change
%.cc %.h:
