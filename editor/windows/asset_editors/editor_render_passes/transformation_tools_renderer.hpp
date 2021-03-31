#pragma once

#include <string>

#include <ovis/rendering/primitive_renderer.hpp>

namespace ovis {
namespace editor {

class TransformationToolsRenderer : public PrimitiveRenderer {
 public:
  static constexpr std::string_view Name() { return "TransformationTools"; }

  TransformationToolsRenderer(Scene* editing_scene);

  void Render(const RenderContext& render_context) override;

 private:
  Scene* editing_scene_;
};

}  // namespace editor
}  // namespace ovis