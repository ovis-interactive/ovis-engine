#include <SDL2/SDL_assert.h>

#include <ovis/core/asset_library.hpp>
#include <ovis/core/scene.hpp>
#include <ovis/graphics/graphics_context.hpp>
#include <ovis/rendering/graphics_loader.hpp>
#include <ovis/rendering/rendering_viewport.hpp>
#include <ovis/imgui/imgui_render_pass.hpp>
#include <ovis/imgui/imgui_start_frame_controller.hpp>

namespace ovis {

ImGuiRenderPass::ImGuiRenderPass() : RenderPass("ImGui") {}

ImGuiRenderPass::~ImGuiRenderPass() {}

void ImGuiRenderPass::CreateResources() {
  SDL_assert(GetEngineAssetLibrary() != nullptr);

  auto start_frame_controller = viewport()->scene()->GetController<ImGuiStartFrameController>();
  SDL_assert(start_frame_controller != nullptr);
  ImGui::SetCurrentContext(start_frame_controller->imgui_context_.get());

  LogI("Loading ImGui resources!");

  shader_program_ = LoadShaderProgram(GetEngineAssetLibrary(), "ui", context());

  unsigned char* pixels;
  int width;
  int height;
  ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  Texture2DDescription font_texture_desc;
  font_texture_desc.width = width;
  font_texture_desc.height = height;
  font_texture_desc.format = TextureFormat::RGBA_UINT8;
  font_texture_desc.filter = TextureFilter::BILINEAR;
  font_texture_desc.mip_map_count = 1;
  font_texture_ = std::make_unique<Texture2D>(context(), font_texture_desc, pixels);

  ImGui::GetIO().Fonts->TexID = font_texture_.get();
}

void ImGuiRenderPass::Render(const RenderContext& render_context) {
  auto start_frame_controller = viewport()->scene()->GetController<ImGuiStartFrameController>();
  if (start_frame_controller == nullptr || !start_frame_controller->frame_started_) {
    LogV("Skipping ImGui rendering!");
    return;
  }

  ImGui::SetCurrentContext(start_frame_controller->imgui_context_.get());

  ImGui::Render();
  ImDrawData* draw_data = ImGui::GetDrawData();

  float L = draw_data->DisplayPos.x;
  float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
  float T = draw_data->DisplayPos.y;
  float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
  const Matrix4 ortho_projection = Matrix4::FromOrthographicProjection(L, R, B, T, -1.0f, 1.0f);

  Mat4x4 projection;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      projection[i][j] = ortho_projection[i][j];
    }
  }
  shader_program_->SetUniform("Projection", projection);

  for (int i = 0; i < draw_data->CmdListsCount; ++i) {
    auto command_list = draw_data->CmdLists[i];

    if (!vertex_buffer_ || !vertex_input_ ||
        vertex_buffer_->description().size_in_bytes < command_list->VtxBuffer.size_in_bytes()) {
      VertexBufferDescription vb_desc;
      vb_desc.size_in_bytes = command_list->VtxBuffer.size_in_bytes();
      vb_desc.vertex_size_in_bytes = sizeof(ImDrawVert);
      vertex_buffer_ = std::make_unique<VertexBuffer>(context(), vb_desc);

      VertexInputDescription vi_desc;
      auto position_location = shader_program_->GetAttributeLocation("Position");
      if (position_location) {
        vi_desc.vertex_attributes.push_back({*position_location, 0, 0, VertexAttributeType::FLOAT32_VECTOR2});
      }
      auto texture_coordinates_location = shader_program_->GetAttributeLocation("TextureCoordinates");
      if (texture_coordinates_location) {
        vi_desc.vertex_attributes.push_back(
            {*texture_coordinates_location, 8, 0, VertexAttributeType::FLOAT32_VECTOR2});
      }
      auto color_location = shader_program_->GetAttributeLocation("Color");
      if (color_location) {
        vi_desc.vertex_attributes.push_back({*color_location, 16, 0, VertexAttributeType::UINT8_NORM_VECTOR4});
      }
      vi_desc.vertex_buffers = {vertex_buffer_.get()};
      vertex_input_ = std::make_unique<VertexInput>(context(), vi_desc);
    }

    if (!index_buffer_ || index_buffer_->description().size_in_bytes < command_list->IdxBuffer.size_in_bytes()) {
      IndexBufferDescription vi_desc;
      vi_desc.size_in_bytes = command_list->IdxBuffer.size_in_bytes();
      vi_desc.index_format = IndexFormat::UINT16;
      index_buffer_ = std::make_unique<IndexBuffer>(context(), vi_desc);
    }

    vertex_buffer_->Write(0, command_list->VtxBuffer.size_in_bytes(), command_list->VtxBuffer.Data);
    index_buffer_->Write(0, command_list->IdxBuffer.size_in_bytes(), command_list->IdxBuffer.Data);

    for (int command_index = 0; command_index < command_list->CmdBuffer.Size; ++command_index) {
      const auto& command = command_list->CmdBuffer[command_index];

      shader_program_->SetTexture("Texture", reinterpret_cast<Texture2D*>(command.TextureId));

      DrawItem draw_item;
      draw_item.vertex_input = vertex_input_.get();
      draw_item.index_buffer = index_buffer_.get();
      draw_item.shader_program = shader_program_.get();
      draw_item.render_target_configuration = viewport()->GetDefaultRenderTargetConfiguration();
      draw_item.start = command.IdxOffset;
      draw_item.count = command.ElemCount;
      draw_item.base_vertex = command.VtxOffset;
      draw_item.primitive_topology = PrimitiveTopology::TRIANGLE_LIST;
      draw_item.blend_state.enabled = true;
      draw_item.blend_state.source_color_factor = SourceBlendFactor::SOURCE_ALPHA;
      draw_item.blend_state.destination_color_factor = DestinationBlendFactor::ONE_MINUS_SOURCE_ALPHA;
      draw_item.scissor_rect.emplace();
      draw_item.scissor_rect->operator[](0) = command.ClipRect.x;
      draw_item.scissor_rect->operator[](1) = command.ClipRect.y;
      draw_item.scissor_rect->operator[](2) = command.ClipRect.z - command.ClipRect.x;
      draw_item.scissor_rect->operator[](3) = command.ClipRect.w - command.ClipRect.y;
      context()->Draw(draw_item);
    }
  }
}

}  // namespace ovis