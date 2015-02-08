// Copyright (c) 2014, Tamas Csala

#include <string>
#include "./font_manager.h"
#include "./font.h"

namespace engine {
namespace gui {

FontManager* FontManager::instance_ = nullptr;

FontData* FontManager::load(const std::string& name, float size) {
  auto font_data = new FontData{name, size};
  instance_->fonts_[{name, size}] = std::unique_ptr<FontData>{font_data};
  return font_data;
}

FontData* FontManager::get(const std::string& name, float size) {
  if (!instance_) { instance_ = new FontManager{}; }

  auto iter = instance_->fonts_.find({name, size});
  if (iter != instance_->fonts_.end()) {
    return iter->second.get();
  } else {
    FontData* font = load(name, size);
    return font;
  }
}

}  // namespace gui
}  // namespace engine
