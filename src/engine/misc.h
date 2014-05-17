#ifndef ENGINE_MISC_H
#define ENGINE_MISC_H

#include <memory>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T{std::forward<Args>(args)...});
}

template<typename T, typename... Args>
std::shared_ptr<T> make_shared(Args&&... args) {
  return std::shared_ptr<T>(new T{std::forward<Args>(args)...});
}

#endif
