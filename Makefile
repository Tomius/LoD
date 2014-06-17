# Copyright (c) 2014, Tamas Csala

BINARY = LoD
SRC_DIR = src
OBJ_DIR = .obj
PRECOMPILED_HEADER_SRC = $(SRC_DIR)/engine/oglwrap_all.h

CPP_FILES := $(shell find -L $(SRC_DIR) -name '*.cc')
OBJECTS := $(subst $(SRC_DIR),$(OBJ_DIR),$(CPP_FILES:.cc=.o))
DEPS := $(OBJECTS:.o=.d)

CXX = clang++
CXX_PRECOMPILED_HEADER_EXTENSION = pch

PRECOMPILED_HEADER = $(PRECOMPILED_HEADER_SRC).$(CXX_PRECOMPILED_HEADER_EXTENSION)
PRECOMPILED_HEADER_DEP = $(subst $(SRC_DIR),$(OBJ_DIR),$(PRECOMPILED_HEADER).d)

BASE_CXXFLAGS = -std=c++11 -Wall -Qunused-arguments \
					 			`pkg-config --cflags glfw3` `Magick++-config --cxxflags --cppflags`

ifeq ($(MAKECMDGOALS),release)
	CXXFLAGS = -O3 -DOGLWRAP_DEBUG=0 $(BASE_CXXFLAGS)
else
	CXXFLAGS = -g -rdynamic $(BASE_CXXFLAGS)
endif

CXXFLAG_PRECOMPILED_HEADER = -include $(PRECOMPILED_HEADER_SRC)

BASE_LDFLAGS =  -lGL -lGLU -lGLEW -lassimp `pkg-config --libs glfw3` \
								-lXxf86vm -lX11 -lXrandr -lXi -lm -lXcursor -lpthread \
								`Magick++-config --ldflags --libs` -lfreetype \
								-Lsrc/engine/gui/freetype-gl -lfreetype-gl

ifeq ($(MAKECMDGOALS),release)
	LDFLAGS = $(BASE_LDFLAGS)
else
	LDFLAGS = -g -rdynamic $(BASE_LDFLAGS)
endif

# Terminal font colors
NORMAL = \e[0m
GREEN = \e[32m
RED = \e[91m
BOLD = \e[1m
YELLOW = \e[93m
CYAN = \e[96m

# The nocolor target is for sublime-text termial, that doesn't support font colors.
ifeq ($(MAKECMDGOALS),nocolor)
	printf = /bin/echo "$(1)$(subst $(OBJ_DIR)/,,$(2))"
else
	printf = /bin/echo -e "$(1)$(3)$(subst $(OBJ_DIR)/,,$(2))$(NORMAL)"
endif

.PHONY: all clean nocolor release

all: $(BINARY)
nocolor: $(BINARY)
release: $(BINARY)

clean:
	@rm -f $(BINARY) -rf $(OBJ_DIR) -f $(PRECOMPILED_HEADER) -f $(PRECOMPILED_HEADER_INSTANTIATE)

ifneq ($(MAKECMDGOALS),clean)
$(shell mkdir -p $(OBJ_DIR))
$(shell echo 0 > $(OBJ_DIR)/objs_current)
$(shell rm -f $(OBJ_DIR)/deps)
endif

# The dependency list files
%.d:
	@ # print some text for the user if its the first .d file
	@ if [ ! -f $(OBJ_DIR)/deps ]; then touch $(OBJ_DIR)/deps; $(call printf,[  0%] ,Calculating CXX dependencies,$(YELLOW)); fi;

	@ # if the file doesn't exist, create its directory,
	@ # else create a .d2 file to sign, that we have just created it (see %.o)
	@ if [ ! -f $@ ]; then mkdir -p $(dir $@); touch $(@:.d=.d2); fi;

	@ # create the dependency list using clang -MM
	@ $(CXX) $(CXXFLAGS) -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.d=.cc)) -MT $(@:.d=.o) -MF $@

ifneq ($(MAKECMDGOALS),clean) # don't create .d files just to remove them...
-include $(DEPS)
endif

# We need a dep list for the precompiled header too
$(PRECOMPILED_HEADER_DEP):
	@ if [ ! -f $(OBJ_DIR)/deps ]; then touch $(OBJ_DIR)/deps; $(call printf,[  0%] ,Calculating CXX dependencies,$(YELLOW)); fi;
	@ if [ ! -f $@ ]; then mkdir -p $(dir $@); touch $(@:.d=.d2); fi;
	@ $(CXX) $(CXXFLAGS) -x c++-header -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.$(CXX_PRECOMPILED_HEADER_EXTENSION).d=)) -MT $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.d=)) -MF $@

ifneq ($(MAKECMDGOALS),clean)
-include $(PRECOMPILED_HEADER_DEP)
endif

# Count the number of objects to be built
ifneq ($(MAKECMDGOALS),clean)
$(shell $(MAKE) -f .MakefileObjsTotal)
$(shell rm -f $(OBJ_DIR)/deps)
$(shell rm -rf .lockdir)
endif

$(PRECOMPILED_HEADER):
	@ $(call printf,$(shell ./.make_get_progress.sh) ,Building CXX precompiled header $@,$(CYAN))
	@ if [ -f $(subst $(SRC_DIR),$(OBJ_DIR),$@).d2 ]; then rm $(subst $(SRC_DIR),$(OBJ_DIR),$@).d2;	else rm -f $(subst $(SRC_DIR),$(OBJ_DIR),$@).d; fi;
	@ $(CXX) $(CXXFLAGS) -x c++-header $(@:.$(CXX_PRECOMPILED_HEADER_EXTENSION)=) -o $@

# The precompiled headers depend on other headers and we need rules for them too
# or else the makefile will go full retard, try to build the headers as they
# object files, and will explode on circular dependencies
%.h:
%.hpp:
%.inl:

# I'm not quite sure about why does the makefile want to build itself
# (it seems that PRECOMPILED_HEADER has Makefile as dependency)
Makefile:
	@ touch $@

%.o: $(PRECOMPILED_HEADER)
	@ $(call printf,$(shell ./.make_get_progress.sh) ,Building CXX object $@,$(GREEN))

	@ # One of the depencies changed, which probably introduced new dependencies,
	@ # that we don't depend on now. So if a new header is included from a header,
	@ # normally we wouldn't track that header's changes, and the make would claim
	@ # that everything is up to date, even if the newly included header changed.
	@ # So we have to recalculate the dependencies for this cc file at next
	@ # compilation. The only way to force that is to remove the outdated .d file.
	@ #
	@ # But note, that we shouldn't remove the .d file, if we have just created it,
	@ # it is guaranteed to be up-to-date then.

	@ # if we have just created the .d file then remove the .d2
	@ # else remove the .d (it's old, and the deps changed, it has to be rebuilt)
	@ if [ -f $(@:.o=.d2) ]; then rm $(@:.o=.d2);	else rm -f $(@:.o=.d); fi;

	@ $(CXX) $(CXXFLAGS) $(CXXFLAG_PRECOMPILED_HEADER) -c $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.o=.cc)) -o $@

$(BINARY): $(OBJECTS)
	@ $(call printf,[100%] ,Linking CXX executable $@,$(BOLD)$(RED))
	@ $(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
