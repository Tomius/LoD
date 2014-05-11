BINARY = LoD
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = .

CPP_FILES = $(wildcard $(SRC_DIR)/*.cc)
OBJECTS = $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cc=.o)))
HEADERS := $(shell find -L $(INCLUDE_DIR) -name '*.h')

MKDIR_P = mkdir -p

CXX = clang++
CXXFLAGS = -std=c++11 -Iinclude/ -Wall -Qunused-arguments \
						`Magick++-config --cxxflags --cppflags` -g -rdynamic
LDFLAGS = -lGL -lGLEW -lsfml-window -lsfml-system -lassimp \
						`Magick++-config --ldflags --libs` -g -rdynamic

.PHONY: all clean

all: $(OBJ_DIR) $(BINARY)

clean:
	rm -f $(BINARY) $(OBJECTS)

$(BINARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	${MKDIR_P} ${OBJ_DIR}
