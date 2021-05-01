#include <SDL2/SDL_assert.h>

#include <ovis/utils/range.hpp>
#include <ovis/core/intersection.hpp>
#include <ovis/core/math.hpp>

namespace ovis {

bool IsConvex(std::span<const Vector2> vertices) {
  // https://stackoverflow.com/questions/471962/how-do-i-efficiently-determine-if-a-polygon-is-convex-non-convex-or-complex
  SDL_assert(vertices.size() >= 3);

  Vector2 prev_prev = vertices[vertices.size() - 2];
  Vector2 prev = vertices[vertices.size() - 1];
  Vector2 old_delta = prev - prev_prev;

  std::optional<bool> signbit;
  for (Vector2 current : vertices) {
    const Vector2 delta = current - prev;
    const float perp_dot = PerpDot(delta, old_delta);

    if (!signbit.has_value()) {
      signbit = std::signbit(perp_dot);
    } else if (*signbit != std::signbit(perp_dot)) {
      return false;
    }

    prev_prev = prev;
    prev = current;
    old_delta = delta;
  }

  return true;
}

size_t GetInsertPosition(std::span<const Vector2> convex_polygon, Vector2 new_position) {
  SDL_assert(convex_polygon.size() >= 3);

  float shortest_distance_squared = std::numeric_limits<float>::infinity();
  size_t best_position;
  Vector2 previous_vertex = convex_polygon.back();
  for (const auto& current_vertex : IndexRange(convex_polygon)) {
    const LineSegment2D edge = {previous_vertex, current_vertex.value()};
    const Vector2 closest_point_on_edge = ComputeClosestPointOnLineSegment(edge, new_position);
    const float squared_distance = SquaredDistance(closest_point_on_edge, new_position);
    if (squared_distance < shortest_distance_squared) {
      best_position = current_vertex.index();
      shortest_distance_squared = squared_distance;
    }
  }

  return best_position;
}

}  // namespace ovis
