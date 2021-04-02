#include "texture_editor.hpp"

#include "../../editor_window.hpp"
#include <fstream>
#include <regex>

#include <ovis/utils/file.hpp>
#include <ovis/utils/range.hpp>
#include <ovis/core/asset_library.hpp>
#include <ovis/core/lua.hpp>
#include <ovis/application/application.hpp>

namespace ovis {
namespace editor {

TextureEditor::TextureEditor(const std::string& texture_id) : AssetEditor(texture_id) {
  // texture_ = EditorWindow::instance()->resource_manager()->Load<Texture2D>()
  texture_ = LoadTexture2D(GetApplicationAssetLibrary(), texture_id, EditorWindow::instance()->context());
  description_ = texture_->description();

  SetFlags(ImGuiWindowFlags_HorizontalScrollbar);
}

void TextureEditor::DrawContent() {
  ImVec2 image_size = {description_.width * scale_, description_.height * scale_};
  ImGui::Image(texture_->id(), image_size);
  if (ImGui::IsItemHovered() && ImGui::GetIO().KeyCtrl) {
    scale_ += ImGui::GetIO().MouseWheel * 0.1f;
  }
}

void TextureEditor::DrawInspectorContent() {
  int width = description_.width;
  ImGui::InputInt("Width", &width, 0, 0, ImGuiInputTextFlags_ReadOnly);

  int height = description_.height;
  ImGui::InputInt("Height", &height, 0, 0, ImGuiInputTextFlags_ReadOnly);

  float scale = scale_ * 100.0f;
  if (ImGui::InputFloat("Scaling", &scale, 0.0f, 0.0f, "%.0f%%")) {
    scale_ = scale / 100.0f;
  }
}

void TextureEditor::Save() {}

}  // namespace editor
}  // namespace ovis