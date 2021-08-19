#pragma once

#include "asset_editor.hpp"

#include <ovis/core/scripting.hpp>

namespace ovis {
namespace editor {

class ScriptLibraryEditor : public AssetEditor {
 public:
  ScriptLibraryEditor(const std::string& asset_id);

  void DrawContent() override;
  void Save() override { SaveFile("json", GetCurrentJsonFileState().dump()); }
  void SetError(ScriptError error) { error_ = std::move(error); }

 private:
  std::optional<ScriptChunk> chunk_;
  json editing_copy_;

  json docs_;
  json::json_pointer current_edit_path_;
  bool start_editing_ = false;
  std::string highlighted_reference_;
  std::string reference_to_highlight_;
  json::json_pointer dragged_action_path_;
  std::optional<ScriptError> error_;
  std::vector<std::string> auto_complete_list_;
  std::vector<json> auto_complete_data_;
  std::string new_action_text_;

  bool DrawEntrypoint();
  bool DrawActions(const json::json_pointer& path, const ScriptActionReference& parent_reference);
  bool DrawAction(const json::json_pointer& path, const ScriptActionReference& reference, bool dragging_preview = false);
  bool DrawFunctionCall(const json::json_pointer& path);
  bool DrawIfStatement(const json::json_pointer& path);
  bool DrawNewAction(const json::json_pointer& path);
  void DrawSpace(const json::json_pointer& path);
  bool DrawStackVariable(const json::json_pointer& path);
  bool DrawInput(const json::json_pointer& path, ScriptTypeId type);

  void BeginNode();
  void EndNode(bool draw_error_border = false);
  bool DrawRenameableValueSource(const char* id, std::string* name, ScriptTypeId type, std::string_view reference);

  void RemoveAction(const json::json_pointer& path);
  void JsonFileChanged(const json& data, const std::string& file_type) override;
  json::json_pointer GetActionWithId(size_t id, json::json_pointer base_path = json::json_pointer{"/actions"});
  std::string GetReferenceName(std::string_view reference);
  size_t GenerateActionId();
};
}  // namespace editor
}  // namespace ovis