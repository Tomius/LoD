all:
	g++ -std=c++11 src/* -o LoD -Iinclude/ -lGL -lGLEW -lsfml-window -lsfml-system `Magick++-config --cppflags --cxxflags --ldflags --libs`
clean:
	rm -rf *o
