Land of Dreams
===

Simple attempt of drawing big terrains in OpenGL with the geomipmapping level of detail algorithm.

Here's a picture of the program's current capability:
![screenshot](http://oi41.tinypic.com/2cbnyb.jpg) 

Dependencies:
-------------
* OpenGL 3.3 + GLEW
* SFML - for window handling
* oglplus - header only c++ wrapper for OpenGL.
* Magick++ - for image loading.


How to build (linux):
--------------------
* Download the terrain files from [**here**](http://dsa11.fileflyer.com/d/c7c1dfe1-7fca-4c38-bcbe-15be87940f71/BFDBe/xLiwCBr/terrain.zip) and extract them into terrain/
* Use the given Makefile, or the prebuilt executeable.

How to build (Windows):
----------------------
* Download the terrain files from [**here**](http://dsa11.fileflyer.com/d/c7c1dfe1-7fca-4c38-bcbe-15be87940f71/BFDBe/xLiwCBr/terrain.zip) and extract them into terrain/
* On Windows there's no uniform way to build, the setup depends on the IDE you use. You will have to link the following libraries:
  * GL
  * GLEW
  * SFML - system/window
  * Magick++

----------------------
If you have any problem post in the issues tab or mail me at tomius1994@gmail.com.
