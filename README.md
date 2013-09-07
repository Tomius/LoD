Land of Dreams
===

Simple attempt of drawing big terrains in OpenGL with the geomipmapping level of detail algorithm. This project also
servers as a test subject for my C++ OpenGL wrapper, oglwrap.

Here's a picture of the program's current capability:
![screenshot](http://oi40.tinypic.com/t71is5.jpg) 

Dependencies:
-------------
* C++11 compiler.
* OpenGL 3.1, GLEW
* [oglwrap](https://github.com/Tomius/oglwrap) - my c++ OpenGL wrapper.
* SFML - for window handling
* Magick++ - for image loading.


How to build (linux):
--------------------
* Get [oglwrap](https://github.com/Tomius/oglwrap) and put it into include/
* Get the external dependencies: libsfml2-dev libmagick++-dev libglew-dev
* Download the terrain files from [**here**](http://dsa11.fileflyer.com/d/c7c1dfe1-7fca-4c38-bcbe-15be87940f71/BFDBe/xLiwCBr/terrain.zip) and extract them into terrain/
* Use the given Makefile, or the prebuilt executeable.

How to build (Windows):
----------------------
* Get [oglwrap](https://github.com/Tomius/oglwrap) and put it into include/
* Download the terrain files from [**here**](http://dsa11.fileflyer.com/d/c7c1dfe1-7fca-4c38-bcbe-15be87940f71/BFDBe/xLiwCBr/terrain.zip) and extract them into terrain/
* You will probably have to change the include paths, for ex. from GL/gl.h to OpenGL/gl.h
* On Windows there's no uniform way to build, the setup depends on the IDE you use. You will have to link the following libraries:
  * GL
  * GLEW
  * SFML - system/window
  * Magick++

----------------------
If you have any problem post in the issues tab or mail me at tomius1994@gmail.com.
