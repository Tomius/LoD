Land of Dreams
===

Simple attempt of drawing big terrains in OpenGL with the geomipmapping level of detail algorithm. The project also heavily involves character animation with GPU skinning. It uses my c++ OpenGL wrapper: oglwrap. 

Here's a picture of the program's current capability:
![screenshot](http://oi41.tinypic.com/iqaqsz.jpg) 

Dependencies:
-------------
* C++11 compiler.
* OpenGL 3.1, GLEW
* [oglwrap](https://github.com/Tomius/oglwrap) - my c++ OpenGL wrapper.
* SFML - for window handling
* Magick++ - for image loading.
* Assimp - for loading the character in.


How to build (linux):
--------------------
* Get [oglwrap](https://github.com/Tomius/oglwrap) and put it into include/
* Get the external dependencies: libsfml2-dev libmagick++-dev libglew-dev libassimp-dev
* Use the given Makefile, or the prebuilt executeable.

How to build (Windows):
----------------------
* Get [oglwrap](https://github.com/Tomius/oglwrap) and put it into include/
* On Windows there's no uniform way to build, the setup depends on the IDE you use. You will have to link the following libraries:
  * GL
  * GLEW
  * SFML - system/window
  * Magick++
  * Assimp

----------------------
If you have any problem post in the issues tab or mail me at tomius1994@gmail.com.
