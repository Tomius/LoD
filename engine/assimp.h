#ifndef ENGINE_ASSIMP_H_
#define ENGINE_ASSIMP_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "oglwrap/glm/glm/glm.hpp"
#include "oglwrap/glm/glm/gtc/type_ptr.hpp"
#include "oglwrap/glm/glm/gtc/matrix_transform.hpp"

// Conversion between assimp and glm matrices

namespace engine {

/// Converts an assimp aiMatrix4x4 to glm mat4
/// @param m - the matrix to convert
static inline glm::mat4 convertMatrix(const aiMatrix4x4& m) {
  return glm::transpose(glm::make_mat4(&m.a1));
}

/// Converts an assimp aiMatrix3x3 to glm mat4
/// @param m - the matrix to convert
static inline glm::mat4 convertMatrix(const aiMatrix3x3& m) {
  return glm::mat4(glm::transpose(glm::make_mat3(&m.a1)));
}

/// Converts a glm mat4 to an assimp aiMatrix4x4
/// @param m - the matrix to convert
static inline aiMatrix4x4 convertMatrix(const glm::mat4& m) {
  return aiMatrix4x4 {
    m[0][0], m[1][0], m[2][0], m[3][0],
    m[0][1], m[1][1], m[2][1], m[3][1],
    m[0][2], m[1][2], m[2][2], m[3][2],
    m[0][3], m[1][3], m[2][3], m[3][3]
  };
}

// Some random templates

template<class T>
/// Interpolates two things.
/// @param a - the first thing.
/// @param b - the second thing.
/// @param alpha - Specifies how 'b' will the result be.
T interpolate(const T& a, const T& b, float alpha) {
  return a + alpha * (b - a);
}

template <typename T>
/// Clamps a value to a given range.
/** @param val - The value to clamp.
  * @param min - The lower bound of the range.
  * @param max - The upper bound of the range. */
inline T clamp(const T& val, const T& min, const T& max) {
  if (val < min) {
    return min;
  } else if (val > max) {
    return max;
  } else {
    return val;
  }
}

}

#endif // ENGINE_ASSIMP_H_
