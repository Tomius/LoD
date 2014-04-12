BINARY = LoD
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))
HEADERS = $(wildcard $(INCLUDE_DIR)/*.hpp)

MKDIR_P = mkdir -p

CXX = clang++
CXXFLAGS = -std=c++11 -Iinclude/ -Wall `Magick++-config --cppflags --cxxflags` -g
LDFLAGS = -lGL -lGLEW -lsfml-window -lsfml-system -lassimp `Magick++-config --ldflags --libs` -g

.PHONY: all clean

all: $(OBJ_DIR) $(BINARY)

clean:
	rm -f $(BINARY) $(OBJECTS)

$(BINARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	${MKDIR_P} ${OBJ_DIR}
