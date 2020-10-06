Land of Dreams
==============

A simple OpenGL 2.1 game engine, written in C++. It features a CDLOD terrain renderer, a mesh loader and render using assimp, GPU skinning animation built on top of that, as well as exponential shadowmaps and several aftereffects (tone mapping, bloom, depth of field, lens flare)

**This is just an educational project.** I don't own the rights for every media in the application.

A screenshot of the engine:
![screenshot](screenshot.png)

Dependencies:
-------------
* C++11 compiler. (clang recommended)
* OpenGL 4.3, Shader model 430
* GLEW - for loading OpenGL extensions
* Magick++ - for image loading.
* Assimp - for loading 3D models.
* FreeType2 - for text rendering (via freetype-gl).
* Bullet - for physics (isn't fully integrated yet).
* Included dependencies:
  * [oglwrap](https://github.com/Tomius/oglwrap) - my C++ OpenGL wrapper.
  * freetype-gl - for GUI texts
  * GLFW3 - for window handling
  * glm - for linear algebra

How to build (Debian/Ubuntu):
--------------------
```
mkdir LandOfDreams && cd LandOfDreams && sudo apt-get install libmagick++-dev libglew-dev libassimp-dev libbullet-dev cmake xorg-dev libglu1-mesa-dev && git clone --recursive https://github.com/Tomius/LoD.git && cd LoD && make -j4 && ./LoD
```

How to build (Linux):
---------------------
* get the external dependencies: libmagick++-dev libglew-dev libassimp-dev libbullet-dev libglm-dev libglfw3-dev cmake xorg-dev libglu1-mesa-dev
* initialize the oglwrap submodule: git submodule init && git submodule update
* build with make (uses clang++), run with ./LoD

How to build (Windows): OUTDATED
-----------------------
* if you downloaded LoD using git, but you didn't use git clone --recursive, then you have to initilaize oglwrap with git submodule init && git submodule update. If you download it via http, you will have to download [oglwrap](https://github.com/Tomius/oglwrap) too, and paste it into src/oglwrap
* dowload all the third party libraries pre-compiled from [here](https://dl.dropboxusercontent.com/u/37129007/LoD/thirdparty.zip)
* add thirdparty/include to compiler search path
* add thirdparty/lib to linker search path
* specify the linker inputs \(thirdparty/lib/linker_inputs\.txt\), or the linker flags \(thirdparty/lib/linker_flags\.txt\)
* enable c++11 mode with your compiler
* compile every .cc file, but none of the .c or .cpp files
* copy the thirdparty/bin files to the exe's directory

Acknowledgements
================
* *Darkz* for useful feedback and help with the fixing of several bugs.
* *Kristóf Marussy (kris7t)* for a lot of useful advices and catching many implementation-dependent bugs.

Inspirations, resources and tools:
---------------------------
* [World Machine](http://www.world-machine.com) - Used for heightmap generation
* [X-blades](http://www.x-blades.com/) - Borrowed Ayumi's character design from this game
* [Autodesk Maya](http://www.autodesk.com/products/autodesk-maya/overview) && [Blender](http://www.blender.org/) - Used for modelling and animating 3D assets.
* [Oglplus sky example](http://oglplus.org/oglplus/html/oglplus_2023_sky_8cpp-example.html) - My skybox is based on this one.
* [Konserian highlands](http://opengameart.org/content/konserian-highlands-set) - Used the trees from this 3D art pack
* [Gamasutra fast blur artice](http://www.gamasutra.com/view/feature/3102/four_tricks_for_fast_blurring_in_.php) - The mipmap based DoF was inspired by this.
* [Filip Strugar's CDLOD paper](http://www.vertexasylum.com/downloads/cdlod/cdlod_latest.pdf) - Implemented the current Level of Detail algorithm for the terrain using this paper (with some changes).
* [Musk's lens flare](https://www.shadertoy.com/view/XdfXRX) - The lens flare I use is based on this.
