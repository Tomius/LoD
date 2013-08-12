#include <SFML/Window.hpp>
#include "oglwrap.hpp"
#include "texture.hpp"
#include "shapes/cube.hpp"


int main() {
    sf::Window window(sf::VideoMode(640, 480), "Dummy Debug Window");
    assert(glewInit() == GLEW_OK);

    return 0;
}


