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
	@rm -f $(BINARY) -rf $(OBJ_DIR) -f $(PRECOMPILED_HEADER)

ifneq ($(MAKECMDGOALS),clean) 						 # don't create .d files just to remove them...
$(shell mkdir -p $(OBJ_DIR))							 # make OBJ_DIR for a helper file
$(shell echo 0 > $(OBJ_DIR)/objs_current)  # reset the built object counter
$(shell touch .MakefileObjsTotal) 				 # force the helper makefile to always run
$(shell $(MAKE) -f .MakefileObjsTotal all) # count the number of objects to be built
$(shell rm -rf $(OBJ_DIR)/deps)						 # remove the dir used to sign the first .d file
$(shell rm -rf .lockdir)									 # reset the lock for .make_get_progress.sh

# include the dependency files
-include $(DEPS)
-include $(PRECOMPILED_HEADER_DEP)
endif

# The dependency list files
%.d:
	@ # print some text for the user if its the first .d file
	@ if mkdir $(OBJ_DIR)/deps 2> /dev/null; then $(call printf,[  0%] ,Calculating CXX dependencies,$(YELLOW)); fi;

	@ # if the file doesn't exist, create its directory,
	@ # else create a .d2 file to sign, that we have just created it (see %.o)
	@ if [ ! -f $@ ]; then mkdir -p $(dir $@); touch $(@:.d=.d2); fi;

	@ # create the dependency list using clang -MM
	@ $(CXX) $(CXXFLAGS) -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.d=.cc)) -MT $(@:.d=.o) -MF $@

	@ # Manually insert the precompiled header as a dependency
	@ sed -i 's,.o: ,.o: $(PRECOMPILED_HEADER) \\\n  ,' $@

# We need a dep list for the precompiled header too
$(PRECOMPILED_HEADER_DEP):
	@ if mkdir $(OBJ_DIR)/deps 2> /dev/null; then $(call printf,[  0%] ,Calculating CXX dependencies,$(YELLOW)); fi;
	@ if [ ! -f $@ ]; then mkdir -p $(dir $@); touch $(@:.d=.d2); fi;
	@ $(CXX) $(CXXFLAGS) -x c++-header -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.$(CXX_PRECOMPILED_HEADER_EXTENSION).d=)) -MT $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.d=)) -MF $@

%.o:
	@ $(call printf,$(shell ./.make_get_progress.sh) ,Building CXX object $@,$(GREEN))

	@ # One of the depencies changed, which probably introduced new dependencies,
	@ # that we don't depend on now. So if a new header is included from a header,
	@ # normally we wouldn't track that header's changes, and the make would claim
	@ # that everything is up to date, even if the newly included header changed.
	@ # So we have to recalculate the dependencies for this cc file at next
	@ # compilation. The easiest to solve this is to  remove the outdated .d file.
	@ # Alternatively we could rebuild the .d right now. I choose the second option
	@ # as it makes the makefile's output nicer (will only print Checking Dependecies)
	@ # at the first compilation.
	@ #
	@ # But note, that we shouldn't remove the .d file, if we have just created it,
	@ # it is guaranteed to be up-to-date then.

	@ # if we have just created the .d file then remove the .d2
	@ # else remove the .d (it's old, and the deps changed, it has to be rebuilt)
	@ #if [ -f $(@:.o=.d2) ]; then rm $(@:.o=.d2);	else rm -f $(@:.o=.d); fi;
	@ if [ -f $(@:.o=.d2) ]; then rm $(@:.o=.d2);	else $(CXX) $(CXXFLAGS) -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.o=.cc)) -MT $@ -MF $(@:.o=.d); sed -i 's,.o: ,.o: $(PRECOMPILED_HEADER) \\\n  ,' $(@:.o=.d); touch $(@:.o=.d2); fi;

	@ $(CXX) $(CXXFLAGS) $(CXXFLAG_PRECOMPILED_HEADER) -c $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.o=.cc)) -o $@

$(PRECOMPILED_HEADER):
	@ $(call printf,$(shell ./.make_get_progress.sh) ,Building CXX precompiled header $@,$(CYAN))
	@ if [ -f $(subst $(SRC_DIR),$(OBJ_DIR),$@).d2 ]; then rm $(subst $(SRC_DIR),$(OBJ_DIR),$@).d2;	else $(CXX) $(CXXFLAGS) -x c++-header -MM $(@:.$(CXX_PRECOMPILED_HEADER_EXTENSION)=) -MT $@ -MF $(subst $(SRC_DIR),$(OBJ_DIR),$@).d; touch $(subst $(SRC_DIR),$(OBJ_DIR),$@).d2; fi;
	@ $(CXX) $(CXXFLAGS) -x c++-header $(@:.$(CXX_PRECOMPILED_HEADER_EXTENSION)=) -o $@

$(BINARY): $(OBJECTS)
	@ $(call printf,[100%] ,Linking CXX executable $@,$(BOLD)$(RED))
	@ $(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%:
