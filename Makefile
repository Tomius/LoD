all:
	g++ -std=c++11 src/* -O3 -o LoD -Iinclude/ -lGL -lGLEW -lsfml-window -lsfml-system -lassimp `Magick++-config --cppflags --cxxflags --ldflags --libs`
clean:
	rm -f *o
