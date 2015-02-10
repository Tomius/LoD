#
# Note: This file was modified by Tamas Csala (removed demo builds)
#
# =========================================================================
# Freetype GL - A C OpenGL Freetype engine
# Platform:    Any
# WWW:         http://code.google.com/p/freetype-gl/
# -------------------------------------------------------------------------
# Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are
# those of the authors and should not be interpreted as representing official
# policies, either expressed or implied, of Nicolas P. Rougier.
# =========================================================================

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.5 )
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules"
                       "${CMAKE_MODULE_PATH}" )

# Get required and optional library
FIND_PACKAGE( OpenGL REQUIRED )
FIND_PACKAGE( Freetype REQUIRED )
FIND_PACKAGE( GLEW REQUIRED )
FIND_LIBRARY( MATH_LIBRARY m )

INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIRS}
                     ${FREETYPE_INCLUDE_DIRS}
                     ${GLEW_INCLUDE_PATHS}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${GLEW_INCLUDE_PATH})


SET( FREETYPE_GL_SRC freetype-gl.h
                     vec234.h
                     opengl.h
                     markup.h
                     mat4.c             mat4.h
                     texture-atlas.c    texture-atlas.h
                     texture-font.c     texture-font.h
                     vertex-buffer.c    vertex-buffer.h
                     vertex-attribute.c vertex-attribute.h
                     font-manager.c     font-manager.h
                     text-buffer.c      text-buffer.h
                     shader.c           shader.h
                     vector.c           vector.h
                     platform.c         platform.h)

ADD_LIBRARY( freetype-gl STATIC ${FREETYPE_GL_SRC} )

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR})
