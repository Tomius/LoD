BINARY = LoD
SRC_DIR = src
OBJ_DIR = obj

CPP_FILES := $(shell find -L $(SRC_DIR) -name '*.cc')
OBJECTS = $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cc=.o)))
DEPS = $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cc=.depends)))
HEADERS := $(shell find -L $(SRC_DIR) -name '*.h')

MKDIR_P = mkdir -p

CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Qunused-arguments \
						`Magick++-config --cxxflags --cppflags` -g -rdynamic
LDFLAGS = -lGL -lGLEW -lsfml-window -lsfml-system -lassimp \
						`Magick++-config --ldflags --libs` -g -rdynamic

.PHONY: all clean

all: $(OBJ_DIR) $(BINARY)

clean:
	rm -f $(BINARY) $(OBJECTS) $(DEPS)

$(OBJ_DIR):
	${MKDIR_P} ${OBJ_DIR}

$(OBJ_DIR)/%.depends: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -MM $< -MT $(OBJ_DIR)/$(notdir $(<:.cc=.o)) > $@

-include $(DEPS)

$(OBJ_DIR)/%.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BINARY): $(DEPS) $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@




