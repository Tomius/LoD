Land of Dreams
===

Simple attempt of drawing big terrains in OpenGL with the geomipmapping level of detail algorithm.

Here's a picture of the program's current capability:
![screenshot](http://oi41.tinypic.com/2cbnyb.jpg) 

Dependencies:
-------------
* OpenGL 3.3+, GLEW
* SFML - for window handling
* oglplus - header only c++ wrapper for OpenGL.
* Magick++ - for image loading.


How to build (linux):
--------------------
* The oglplus is header-only, but has to be configured, so I can't just put it into 
include folder. You have to  
 * git clone https://github.com/matus-chochlik/oglplus.git
 * cd oglplus 
 * ./configure.sh [--prefix=/path/to/LoD/include] --no-docs --no-examples --build-and-install
* Get the other dependencies: libsfml-dev libmagick++-dev libglew-dev
* Download the terrain files from [**here**](http://dsa11.fileflyer.com/d/c7c1dfe1-7fca-4c38-bcbe-15be87940f71/BFDBe/xLiwCBr/terrain.zip) and extract them into terrain/
* Use the given Makefile, or the prebuilt executeable.

How to build (Windows):
----------------------
* The oglplus is header-only, but has to be configured, so I can't just put it into 
include folder. You have to dowload it from https://github.com/matus-chochlik/oglplus.git
and run the configure.bat
* Download the terrain files from [**here**](http://dsa11.fileflyer.com/d/c7c1dfe1-7fca-4c38-bcbe-15be87940f71/BFDBe/xLiwCBr/terrain.zip) and extract them into terrain/
* You will probably have to change the include paths, for ex. from GL/gl.h to OpenGL/gl.h
* On Windows there's no uniform way to build, the setup depends on the IDE you use. You will have to link the following libraries:
  * GL
  * GLEW
  * SFML - system/window
  * Magick++

----------------------
If you have any problem post in the issues tab or mail me at tomius1994@gmail.com.
