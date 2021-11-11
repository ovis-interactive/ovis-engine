#pragma once

#include <ovis/core/scene_controller.hpp>

namespace ovis {

class AnimatorController : public SceneController {
 public:
  AnimatorController() : SceneController("AnimatorController") {}
  void Update(std::chrono::microseconds delta_time) override;

 private:
};

}  // namespace ovis

