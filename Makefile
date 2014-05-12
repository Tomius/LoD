BINARY = LoD
SRC_DIR = src
OBJ_DIR = obj

CPP_FILES := $(shell find -L $(SRC_DIR) -name '*.cc')
OBJECTS := $(subst $(SRC_DIR),$(OBJ_DIR),$(CPP_FILES:.cc=.o))
DEPS := $(subst $(SRC_DIR),$(OBJ_DIR),$(CPP_FILES:.cc=.dep))
HEADERS := $(shell find -L $(SRC_DIR) -name '*.h')

CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Qunused-arguments \
						`Magick++-config --cxxflags --cppflags` -g -rdynamic
LDFLAGS = -lGL -lGLEW -lsfml-window -lsfml-system -lassimp \
						`Magick++-config --ldflags --libs` -g -rdynamic

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
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@




