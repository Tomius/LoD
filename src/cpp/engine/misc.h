// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_MISC_H
#define ENGINE_MISC_H

#include <memory>

namespace engine {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>{new T(std::forward<Args>(args)...)};
}

template<typename T>
std::unique_ptr<T> make_unique() {
  return std::unique_ptr<T>{new T{}};
}

template<typename T>
T sqr(const T& x) {
  return x*x;
}

}  // namespace engine

#endif
