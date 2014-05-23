Land of Dreams
===

Simple attempt of drawing big terrains in OpenGL with the geomipmapping level of detail algorithm. The project also heavily involves character animation with GPU skinning. It uses my c++ OpenGL wrapper: oglwrap.

**This is just an educational project.** I don't own the rights for every media in the application.

Here's a picture of the program's current capability (click on it, to see a short video):
[![screenshot](screenshot.png)](https://www.youtube.com/watch?v=ef93WR3KzaE)

Dependencies:
-------------
* C++11 compiler.
* OpenGL 2.1, Shader model 120
* GLEW - for loading OpenGL extensions
* [oglwrap](https://github.com/Tomius/oglwrap) - my C++ OpenGL wrapper.
* GLFW3 - for window handling
* Magick++ - for image loading.
* Assimp - for loading the character in. (An up-to-date copy of assimp is prefered, or else Ayumi might not get loaded correctly.)


How to build (Debian/Ubuntu):
--------------------
```
mkdir LandOfDreams && cd LandOfDreams && sudo apt-get install libmagick++-dev libglew-dev && git clone https://github.com/assimp/assimp.git && cd assimp && cmake . && sudo make -j4 install && cd .. && git clone https://github.com/glfw/glfw.git && cd glfw && cmake . && sudo make -j4 install && git clone --recursive https://github.com/Tomius/LoD.git && cd LoD && make -j4 && ./LoD
```

How to build (linux):
---------------------
* get the external dependencies: libmagick++-dev libglew-dev [assimp](https://github.com/assimp/assimp) [glfw](https://github.com/glfw/glfw)
* initialize the oglwrap submodule: git submodule init && git submodule update
* build with make (uses clang++), run with ./LoD

How to build (Windows):
----------------------
* Get the 3rd party libraries:
  * [GLEW](http://sourceforge.net/projects/glew/files/glew/1.10.0/) - The prebulilt one only works with MSVC, you probably need to build it yourself.
  * [GLFW](https://github.com/glfw/glfw)
  * [Magick++](http://www.imagemagick.org/script/binary-releases.php) - Download the dynamic link Q16, install to compiler's path
  * [Assimp](http://sourceforge.net/projects/assimp/files/assimp-3.0/) - You need the SDK exe.
* Add the librarires to link:
  * glew32
  * opengl32
  * glfw3dll
  * assimp
* Add ./include to search directories, or specify -Iinclude compiler flag.
* Add `Magick++-config --cppflags --cxxflags --ldflags` to compiler settings
* Add `Magick++-config --libs` to linker settings
* Enable c++11 mode with your compiler

Acknowledgements
================

- *Darkz* for useful feedback and help with the fixing of several bugs.
- *Kristóf Marussy (kris7t)* for a lot of useful advices and catching many implementation-dependent bugs.

----------------------
If you have any problem, please post in the issues tab or mail me at tomius1994@gmail.com. Any feedback would be appreciated.
