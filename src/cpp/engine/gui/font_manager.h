// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GUI_FONT_MANAGER_H_
#define ENGINE_GUI_FONT_MANAGER_H_

#include <map>
#include <memory>
#include <string>
#include <utility>

namespace engine {
namespace gui {

struct FontCompare {
  bool operator() (const std::pair<std::string, float>& x,
                   const std::pair<std::string, float>& y) const {
    if (x.first < y.first) {
      return true;
    } else if (x.first > y.first) {
      return false;
    } else {
      return x.second + 1e-3f < y.second;
    }
  }
};

class FontData;

class FontManager {
  static FontManager* instance_;
  std::map<std::pair<std::string, float>,
           std::unique_ptr<FontData>, FontCompare> fonts_;

  static FontData* load(const std::string& name, float size);
 public:
  static FontData* get(const std::string& name, float size);
};

}  // namespace gui
}  // namespace engine

#endif
