#pragma once

#include <string_view>

#include <ovis/utils/safe_pointer.hpp>
#include <ovis/core/intersection.hpp>
#include <ovis/core/scene.hpp>
#include <ovis/core/scene_controller.hpp>
#include <ovis/editor_viewport/viewport_controller.hpp>

namespace ovis {
namespace editor {

class ObjectSelectionController : public ViewportController {
 public:
  static constexpr std::string_view Name() { return "ObjectSelectionController"; }

  ObjectSelectionController();

  void Update(std::chrono::microseconds) override;
  void ProcessEvent(Event* event) override;

  void SelectObject(std::string_view object_path);
  void SelectObject(SceneObject* object);
  void ClearSelection() { SelectObject(nullptr); }

  inline bool has_selected_object() const { return selected_object_; }
  inline SceneObject* selected_object() const { return selected_object_.get(); }
  AxisAlignedBoundingBox3D selected_object_aabb() const { return selected_object_aabb_; }

 private:
  safe_ptr<SceneObject> selected_object_;
  AxisAlignedBoundingBox3D selected_object_aabb_;
};

}  // namespace editor
}  // namespace ovis
