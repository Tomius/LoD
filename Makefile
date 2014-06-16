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

ifeq ($(MAKECMDGOALS),release)
	CXXFLAGS = -O3 -DOGLWRAP_DEBUG=0 $(BASE_CXXFLAGS)
else
	CXXFLAGS = -g -rdynamic $(BASE_CXXFLAGS)
endif

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

# The nocolor target is for sublime-text termial, that doesn't support font colors.
ifeq ($(MAKECMDGOALS),nocolor)
	printf = $(info $(1)$(subst $(OBJ_DIR)/,,$(2)))
else
	printf = @/bin/echo -e "$(1)$(3)$(subst $(OBJ_DIR)/,,$(2))$(NORMAL)"
endif

.PHONY: all clean nocolor release

all: $(BINARY)
nocolor: $(BINARY)
release: $(BINARY)

clean:
	@rm -f $(BINARY) -rf $(OBJ_DIR)

# The dependency list files
%.d:
	$(call printf,,Creating CXX dependency list $@,$(YELLOW))

	@ # if the file doesn't exist, create its directory,
	@ # else create a .d2 file to sign, that we have just created it (see %.o)
	@ if [ ! -f $@ ]; then mkdir -p $(dir $@); touch $(@:.d=.d2); fi;

	@ # create the dependency list using clang -MM
	@ $(CXX) $(CXXFLAGS) -MM $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.d=.cc)) -MT $(@:.d=.o) -MF $@

ifneq ($(MAKECMDGOALS),clean) # don't create .d files just to remove them...
-include $(DEPS)
endif

ifneq ($(MAKECMDGOALS),clean)
$(shell $(MAKE) -f .MakefileObjsTotal)
$(shell echo 0 > $(OBJ_DIR)/objs_current)
$(shell rm -rf .lockdir)
endif

%.o:
	$(call printf,$(shell ./.make_get_progress.sh) ,Building CXX object $@,$(GREEN))

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

	@ $(CXX) $(CXXFLAGS) -c $(subst $(OBJ_DIR),$(SRC_DIR),$(@:.o=.cc)) -o $@

$(BINARY): $(OBJECTS)
	$(call printf,[100%] ,Linking CXX executable $@,$(BOLD)$(RED))
	@ $(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
