#include "selected_object_bounding_box.hpp"

#include "../editing_controllers/object_selection_controller.hpp"

#include <ovis/core/transform.hpp>
#include <ovis/rendering/rendering_viewport.hpp>

namespace ovis {
namespace editor {

SelectedObjectBoundingBox::SelectedObjectBoundingBox(Scene* editing_scene)
    : PrimitiveRenderer("SelectedObjectBoundingBox"), editing_scene_(editing_scene) {}

void SelectedObjectBoundingBox::Render(const RenderContext& render_context) {
  auto* object_selection_controller =
      editing_scene_->GetController<ObjectSelectionController>("ObjectSelectionController");
  SceneObject* scene_object = object_selection_controller->selected_object();

  if (scene_object != nullptr) {
    BeginDraw(render_context);

    Transform* transform = scene_object->GetComponent<Transform>("Transform");
    if (transform != nullptr) {
      const Vector3 normalied_device_coordinates =
          TransformPosition(render_context.world_to_view_space, transform->position());
      const Vector2 device_coordinates =
          viewport()->NormalizedDeviceCoordinatesToDeviceCoordinates(normalied_device_coordinates);
      DrawDisc(Vector3{device_coordinates.x, device_coordinates.y, 0.0f}, 20.0f, Color::Red());
      LogV("Drawing disc at {}", device_coordinates);
    }

    EndDraw();
  }
}

}  // namespace editor
}  // namespace ovis
