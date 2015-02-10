SRC_DIR = src/cpp
OBJ_DIR = .obj
PRECOMPILED_HEADER_SRC = $(SRC_DIR)/engine/oglwrap_all.h
CXX_PRECOMPILED_HEADER_EXTENSION = pch
PRECOMPILED_HEADER = $(PRECOMPILED_HEADER_SRC).$(CXX_PRECOMPILED_HEADER_EXTENSION)
PRECOMPILED_HEADER_DEP = $(subst $(SRC_DIR),$(OBJ_DIR),$(PRECOMPILED_HEADER).d)

TP_DIR = thirdparty
FREETYPE_GL_DIR = $(TP_DIR)/freetype-gl
FREETYPE_GL_ARCHIVE = $(FREETYPE_GL_DIR)/libfreetype-gl.a
GLFW_DIR = $(TP_DIR)/glfw
GLFW_ARCHIVE = $(GLFW_DIR)/src/libglfw3.a

CPP_FILES := $(shell find -L $(SRC_DIR) -name '*.cc')
OBJECTS := $(subst $(SRC_DIR),$(OBJ_DIR),$(CPP_FILES:.cc=.o))
DEPS := $(OBJECTS:.o=.d)

$(shell echo 0 > $(OBJ_DIR)/objs_total)
-include $(DEPS)
-include $(PRECOMPILED_HEADER_DEP)

.PHONY: all

all: $(PRECOMPILED_HEADER) $(OBJECTS) $(FREETYPE_GL_ARCHIVE) $(GLFW_ARCHIVE) progress_counter.make

$(OBJECTS):
	@echo $$((`cat $(OBJ_DIR)/objs_total` + 1)) > $(OBJ_DIR)/objs_total

$(FREETYPE_GL_ARCHIVE):
	@echo $$((`cat $(OBJ_DIR)/objs_total` + 1)) > $(OBJ_DIR)/objs_total

$(GLFW_ARCHIVE):
	@echo $$((`cat $(OBJ_DIR)/objs_total` + 1)) > $(OBJ_DIR)/objs_total

$(PRECOMPILED_HEADER):
	@echo $$((`cat $(OBJ_DIR)/objs_total` + 1)) > $(OBJ_DIR)/objs_total
	@rm -f $(OBJECTS) # have to rebuild every object

%.h:
	@
%.hpp:
	@
%.inl:
	@
%:
	@
