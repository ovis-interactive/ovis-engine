#include "gizmo_controller.hpp"

#include "object_selection_controller.hpp"

#include <imgui.h>

#include <ovis/utils/log.hpp>
#include <ovis/core/intersection.hpp>
#include <ovis/core/scene_viewport.hpp>
#include <ovis/core/transform.hpp>
#include <ovis/input/mouse_button.hpp>

namespace ovis {
namespace editor {

float DistanceToLineSegment(Vector2 point, const LineSegment2D& line_segment) {
  // https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
  const Vector2 line_segment_vector = line_segment.endpoints[1] - line_segment.endpoints[0];
  const float squared_line_segment_length = SquaredLength(line_segment_vector);
  if (squared_line_segment_length == 0.0f) {
    return Length(point - line_segment.endpoints[0]);
  }

  const float t =
      clamp(Dot(point - line_segment.endpoints[0], line_segment_vector) / squared_line_segment_length, 0.0f, 1.0f);
  const Vector2 projected_point_on_line_segment = line_segment.endpoints[0] + t * line_segment_vector;
  return Length(point - projected_point_on_line_segment);
}

GizmoController::GizmoController(Scene* game_scene) : SceneController("GizmoController"), game_scene_(game_scene) {
  SubscribeToEvent(MouseMoveEvent::TYPE);
  SubscribeToEvent(MouseButtonPressEvent::TYPE);
  SubscribeToEvent(MouseButtonReleaseEvent::TYPE);
}

void GizmoController::Update(std::chrono::microseconds) {
  auto* object_selection_controller = scene()->GetController<ObjectSelectionController>("ObjectSelectionController");
  SDL_assert(object_selection_controller != nullptr);

  SceneObject* scene_object = object_selection_controller->selected_object();
  if (!scene_object) {
    object_selected_ = false;
    return;
  }

  Transform* transform = scene_object->GetComponent<Transform>("Transform");
  if (!transform) {
    object_selected_ = false;
    return;
  }
  object_selected_ = true;

  auto viewport = game_scene_->main_viewport();
  object_position_world_space_ = transform->position();
  object_position_screen_space_ = viewport->WorldSpacePositionToScreenSpace(object_position_world_space_);

  Vector2 position2d = object_position_screen_space_;

  const Quaternion rotation = transform->rotation();

  const Vector3 unit_offset = viewport->WorldSpacePositionToScreenSpace(transform->position() + Vector3::PositiveX());
  world_to_screen_space_factor_ = Length(unit_offset - object_position_screen_space_);

  x_axis_world_space_ =
      (gizmo_radius_screen_space_ / world_to_screen_space_factor_) * (rotation * Vector3::PositiveX());
  y_axis_world_space_ =
      (gizmo_radius_screen_space_ / world_to_screen_space_factor_) * (rotation * Vector3::PositiveY());
  z_axis_world_space_ =
      (gizmo_radius_screen_space_ / world_to_screen_space_factor_) * (rotation * Vector3::PositiveZ());

  x_axis_endpoint_screen_space_ =
      viewport->WorldSpacePositionToScreenSpace(object_position_world_space_ + x_axis_world_space_);
  y_axis_endpoint_screen_space_ =
      viewport->WorldSpacePositionToScreenSpace(object_position_world_space_ + y_axis_world_space_);
  z_axis_endpoint_screen_space_ =
      viewport->WorldSpacePositionToScreenSpace(object_position_world_space_ + z_axis_world_space_);

  if (current_tooltip_.length() != 0) {
    ImGui::SetTooltip("%s", current_tooltip_.c_str());
  }
}

void GizmoController::ProcessEvent(Event* event) {
  if (!object_selected_) {
    return;
  }

  if (event->type() == MouseMoveEvent::TYPE) {
    auto mouse_move_event = static_cast<MouseMoveEvent*>(event);

    if (!GetMouseButtonState(MouseButton::Left())) {
      if (CheckMousePosition(mouse_move_event->screen_space_position())) {
        event->StopPropagation();
      }
    } else {
      if (selected_axes_ != AxisSelection::NOTHING) {
        HandleDragging(mouse_move_event);
      }
    }
  } else if (event->type() == MouseButtonPressEvent::TYPE) {
    auto button_press_event = static_cast<MouseButtonPressEvent*>(event);
    const Vector2 position = button_press_event->screen_space_position();
    if (CheckMousePosition(position)) {
      event->StopPropagation();
    }
  } else if (event->type() == MouseButtonReleaseEvent::TYPE) {
    auto button_release_event = static_cast<MouseButtonReleaseEvent*>(event);
    current_tooltip_ = "";
    if (selected_axes_ != AxisSelection::NOTHING) {
      event->StopPropagation();
    }
    // We may hover another gizmo now, so recheck it
    CheckMousePosition(button_release_event->screen_space_position());
  }
}

bool GizmoController::CheckMousePosition(Vector2 position) {
  switch (gizmo_type()) {
    case GizmoController::GizmoType::MOVE:
      [[fallthrough]];
    case GizmoType::SCALE:
      if (Length(position - static_cast<Vector2>(object_position_screen_space_)) <= point_size_screen_space_) {
        selected_axes_ = AxisSelection::XYZ;
        return true;
      } else if (DistanceToLineSegment(position, {object_position_screen_space_, x_axis_endpoint_screen_space_}) <=
                 line_thickness_screen_space_) {
        selected_axes_ = AxisSelection::X;
        return true;
      } else if (DistanceToLineSegment(position, {object_position_screen_space_, y_axis_endpoint_screen_space_}) <=
                 line_thickness_screen_space_) {
        selected_axes_ = AxisSelection::Y;
        return true;
      } else if (DistanceToLineSegment(position, {object_position_screen_space_, z_axis_endpoint_screen_space_}) <=
                 line_thickness_screen_space_) {
        selected_axes_ = AxisSelection::Z;
        return true;
      } else {
        selected_axes_ = AxisSelection::NOTHING;
        return false;
      }

    case GizmoType::ROTATE:
      auto viewport = game_scene_->main_viewport();
      const Ray3D view_ray = viewport->CalculateViewRay(position);
      auto hitting_rotate_gizmo = [this, view_ray, viewport](Vector3 rotation_axis) -> bool {
        const Plane3D plane = Plane3D::FromPointAndNormal(object_position_world_space_, rotation_axis);
        auto intersection = ComputeRayPlaneIntersection(view_ray, plane);

        if (!intersection) {
          return false;
        }

        const float distance = Distance(*intersection, object_position_world_space_);
        return world_to_screen_space_factor_ *
                   std::abs(distance - gizmo_radius_screen_space_ / world_to_screen_space_factor_) <=
               line_thickness_screen_space_;
      };

      if (hitting_rotate_gizmo(x_axis_world_space_)) {
        selected_axes_ = AxisSelection::X;
        return true;
      } else if (hitting_rotate_gizmo(y_axis_world_space_)) {
        selected_axes_ = AxisSelection::Y;
        return true;
      } else if (hitting_rotate_gizmo(z_axis_world_space_)) {
        selected_axes_ = AxisSelection::Z;
        return true;
      } else {
        selected_axes_ = AxisSelection::NOTHING;
        return false;
      }
  }
}

void GizmoController::HandleDragging(MouseMoveEvent* mouse_move_event) {
  auto* object_selection_controller = scene()->GetController<ObjectSelectionController>("ObjectSelectionController");
  SDL_assert(object_selection_controller != nullptr);
  SceneObject* scene_object = object_selection_controller->selected_object();
  SDL_assert(scene_object);
  Transform* transform = scene_object->GetComponent<Transform>("Transform");
  SDL_assert(transform);

  const Vector2 current_mouse_position_screen_space = mouse_move_event->screen_space_position();
  const Vector2 previous_mouse_position_screen_space =
      current_mouse_position_screen_space - mouse_move_event->relative_screen_space_position();

  switch (gizmo_type()) {
    case GizmoController::GizmoType::MOVE: {
      Vector3 world_space_direction = Vector3::Zero();
      if (selected_axes_ == AxisSelection::X) {
        world_space_direction = x_axis_world_space_;
      } else if (selected_axes_ == AxisSelection::Y) {
        world_space_direction = y_axis_world_space_;
      } else if (selected_axes_ == AxisSelection::Z) {
        world_space_direction = z_axis_world_space_;
      }

      const Vector3 previous_mouse_world_space_position = mouse_move_event->viewport()->ScreenSpacePositionToWorldSpace(
          Vector3::FromVector2(previous_mouse_position_screen_space, object_position_screen_space_.z));
      const Vector3 current_mouse_world_space_position = mouse_move_event->viewport()->ScreenSpacePositionToWorldSpace(
          Vector3::FromVector2(current_mouse_position_screen_space, object_position_screen_space_.z));
      const Vector3 world_space_offset = current_mouse_world_space_position - previous_mouse_world_space_position;

      Vector2 screen_space_offset = Vector3::Zero();
      if (SquaredLength(world_space_direction) > 0) {
        const Vector3 normalized_world_space_direction = Normalize(world_space_direction);
        const float t = Dot(normalized_world_space_direction, world_space_offset);
        transform->Move(t * normalized_world_space_direction);
      } else if (selected_axes_ != AxisSelection::NOTHING) {
        transform->Move(world_space_offset);
      }
      current_tooltip_ = fmt::format("{}", transform->position());
      break;
    }

    case GizmoType::ROTATE: {
      Vector3 rotation_axis;
      if (selected_axes_ == AxisSelection::X) {
        rotation_axis = x_axis_world_space_;
      } else if (selected_axes_ == AxisSelection::Y) {
        rotation_axis = y_axis_world_space_;
      } else if (selected_axes_ == AxisSelection::Z) {
        rotation_axis = z_axis_world_space_;
      }

      const Plane3D plane = Plane3D::FromPointAndNormal(object_position_world_space_, rotation_axis);
      const auto previous_intersection = ComputeRayPlaneIntersection(
          mouse_move_event->viewport()->CalculateViewRay(previous_mouse_position_screen_space), plane);
      const auto current_intersection = ComputeRayPlaneIntersection(
          mouse_move_event->viewport()->CalculateViewRay(current_mouse_position_screen_space), plane);

      if (!previous_intersection || !current_intersection) {
        // I don't know what to do here
        return;
      }

      const Vector3 previous_intersection_direction = Normalize(*previous_intersection - object_position_world_space_);
      const Vector3 current_intersection_direction = Normalize(*current_intersection - object_position_world_space_);
      const float sign = Dot(Cross(previous_intersection_direction, current_intersection_direction), rotation_axis);
      const float absolute_angle = std::acos(Dot(previous_intersection_direction, current_intersection_direction));
      const float angle = std::copysign(absolute_angle, sign);
      transform->Rotate(Normalize(rotation_axis), angle);
      const auto q = Quaternion::FromAxisAndAngle(Normalize(rotation_axis), angle);
      current_tooltip_ = fmt::format("{}\n{}\n{}\n{}°", transform->rotation(), q, q * transform->rotation(),
                                     angle * RadiansToDegreesFactor<float>());
      break;
    }

    case GizmoType::SCALE:
      break;
  }
}

}  // namespace editor
}  // namespace ovis
