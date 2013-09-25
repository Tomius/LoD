#pragma once

struct svec2 {
  short x, y;

  svec2() :x(0), y(0) {}

  svec2(short a, short b)
    : x(a), y(b) {}

  svec2 operator+(const svec2 rhs) {
    return svec2(x + rhs.x, y + rhs.y);
  }
};

inline svec2 operator*(short lhs, const svec2 rhs) {
  return svec2(lhs * rhs.x, lhs * rhs.y);
}
