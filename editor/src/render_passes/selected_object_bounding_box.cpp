#include "ovis/editor/render_passes/selected_object_bounding_box.hpp"

#include "ovis/core/transform.hpp"
#include "ovis/rendering/rendering_viewport.hpp"
#include "ovis/rendering2d/renderer2d.hpp"
#include "ovis/editor/editor_viewport.hpp"
#include "ovis/editor/object_selection_controller.hpp"

namespace ovis {
namespace editor {

SelectedObjectBoundingBox::SelectedObjectBoundingBox() {
  RenderAfter<Renderer2D>();
}

void SelectedObjectBoundingBox::Render(const RenderContext& render_context) {
  // auto* object_selection_controller = EditorViewport::instance()->object_selection_controller();
  // SceneObject* object = object_selection_controller->selected_object();

  // if (object != nullptr) {
  //   AxisAlignedBoundingBox3D aabb = object_selection_controller->selected_object_aabb();

  //   BeginDraw(render_context);

  //   Transform* transform = object->GetComponent<Transform>();
  //   const Matrix3x4 local_to_world =
  //       transform ? transform->local_to_world_matrix() : Matrix3x4::IdentityTransformation();
  //   std::array<Vector3, 8> aabb_vertices;
  //   for (int i = 0; i < 8; ++i) {
  //     aabb_vertices[i] =
  //         TransformPosition(local_to_world, aabb.center + 2.0f * Vector3::UnitCube()[i] * aabb.half_extend);
  //   }

  //   DrawLine(aabb_vertices[4], aabb_vertices[5], Color::White());
  //   DrawLine(aabb_vertices[6], aabb_vertices[7], Color::White());
  //   DrawLine(aabb_vertices[4], aabb_vertices[6], Color::White());
  //   DrawLine(aabb_vertices[5], aabb_vertices[7], Color::White());
  //   DrawLine(aabb_vertices[0], aabb_vertices[1], Color::White());
  //   DrawLine(aabb_vertices[2], aabb_vertices[3], Color::White());
  //   DrawLine(aabb_vertices[0], aabb_vertices[2], Color::White());
  //   DrawLine(aabb_vertices[1], aabb_vertices[3], Color::White());

  //   EndDraw();
  // }
}

}  // namespace editor
}  // namespace ovis
