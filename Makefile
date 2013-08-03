all:
	g++ -std=c++11 *.cpp -o LoD -lGL -lGLEW -lsfml-window -lsfml-system `Magick++-config --cppflags --cxxflags --ldflags --libs`
clean:
	rm -rf *o
