#include "editor_window_controller.hpp"

#include "editor_asset_library.hpp"
#include "editor_window.hpp"
#include "windows/asset_editors/asset_editor.hpp"
#include "windows/asset_importers/asset_importer.hpp"

#include <emscripten/html5.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <ovis/core/asset_library.hpp>
#include <ovis/core/log.hpp>
#include <ovis/engine/engine.hpp>
#include <ovis/engine/scene.hpp>
#include <ovis/engine/window.hpp>

namespace ove {

EditorWindowController::EditorWindowController(const std::vector<std::string>* log_history)
    : ovis::SceneController("EditorWindowController"), log_window_(log_history), asset_viewer_window_(&open_editors_) {
  ovis::CreateApplicationAssetLibrary<EditorAssetLibrary>("/assets/");

  icons_.save = ovis::LoadTexture2D("icon-save", EditorWindow::instance()->context());
  icons_.undo = ovis::LoadTexture2D("icon-undo", EditorWindow::instance()->context());
  icons_.redo = ovis::LoadTexture2D("icon-redo", EditorWindow::instance()->context());
}

void EditorWindowController::Update(std::chrono::microseconds delta_time) {
  if (AssetEditor::last_focused_document_window != nullptr) {
    AssetEditor::last_focused_document_window->Update(delta_time);
  }
}

void EditorWindowController::DrawImGui() {
  DrawToolbar();
  DrawDockSpace();
}

bool EditorWindowController::ProcessEvent(const SDL_Event& event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_s && (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) != 0) {
      Save();
      return true;
    } else if (event.key.keysym.sym == SDLK_z && (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) != 0 &&
               (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) != 0) {
      Redo();
      return true;
    } else if (event.key.keysym.sym == SDLK_z && (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) != 0) {
      Undo();
      return true;
    }
  } else if (event.type == SDL_DROPFILE) {
    const std::string filename = event.drop.file;
    SDL_free(event.drop.file);
    ImportAsset(filename);
    return true;
  }

  if (AssetEditor::last_focused_document_window) {
    return AssetEditor::last_focused_document_window->ProcessEvent(event);
  } else {
    return false;
  }
}

void EditorWindowController::DrawDockSpace() {
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + icon_size_.y + 10));
  ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - icon_size_.y - 10));
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Ovis Editor", nullptr, window_flags);
  ImGui::PopStyleVar(3);

  if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockspace")) == NULL) {
    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::DockBuilderRemoveNode(dockspace_id);  // Clear out existing layout
    ImGui::DockBuilderAddNode(dockspace_id,
                              ImGuiDockNodeFlags_DockSpace);  // Add empty node
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

    // This variable will track the document node, however we
    // are not using it here as we aren't docking anything
    // into it.
    ImGuiID dock_main_id = dockspace_id;

    ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, NULL, &dock_main_id);
    ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 1.0f / 4.0f, NULL, &dock_main_id);
    ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, NULL, &dock_main_id);

    ImGui::DockBuilderDockWindow("Scene Objects", dock_id_left);
    ImGui::DockBuilderDockWindow("Object Properties", dock_id_right);
    ImGui::DockBuilderDockWindow("Scene Properties", dock_id_right);
    ImGui::DockBuilderDockWindow("Log", dock_id_bottom);
    ImGui::DockBuilderDockWindow("Assets", dock_id_bottom);

    ImGui::DockBuilderFinish(dockspace_id);
  }

  ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, 0xff000000);
  ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
  ImGui::PopStyleColor();

  for (auto& document : open_editors_) {
    document->Draw(dockspace_id);
  }

  open_editors_.erase(std::remove_if(open_editors_.begin(), open_editors_.end(),
                                     [](const auto& document) { return document->should_close(); }),
                      open_editors_.end());

  if (AssetEditor::last_focused_document_window != nullptr) {
    AssetEditor::last_focused_document_window->DrawPropertyWindows();
  }

  ImGui::End();

  log_window_.Draw();
  asset_viewer_window_.Draw();
}

void EditorWindowController::DrawToolbar() {
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
  ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, icon_size_.y + 10));
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                  ImGuiWindowFlags_NoSavedSettings;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Toolbar", nullptr, window_flags);
  ImGui::PopStyleVar(3);

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(48, 48, 48)));

  if (ImGui::ImageButton(icons_.save.get(), icon_size_)) {
    Save();
  }
  if (ImGui::IsItemHovered()) {
    if (AssetEditor::last_focused_document_window != nullptr) {
      ImGui::SetTooltip("Save: %s", AssetEditor::last_focused_document_window->asset_id().c_str());
    } else {
      ImGui::SetTooltip("Nothing to save");
    }
  }

  ImGui::SameLine();
  if (ImGui::ImageButton(icons_.undo.get(), icon_size_)) {
    Undo();
  }
  if (ImGui::IsItemHovered()) {
    std::string undo_description = "Nothing to undo";
    if (AssetEditor::last_focused_document_window != nullptr &&
        AssetEditor::last_focused_document_window->GetActionHistory()->undo_possible()) {
      undo_description = AssetEditor::last_focused_document_window->GetActionHistory()->undo_description();
    }
    ImGui::SetTooltip("Undo: %s", undo_description.c_str());
  }

  ImGui::SameLine();
  if (ImGui::ImageButton(icons_.redo.get(), icon_size_)) {
    Redo();
  }
  if (ImGui::IsItemHovered()) {
    std::string redo_description = "Nothing to redo";
    if (AssetEditor::last_focused_document_window != nullptr &&
        AssetEditor::last_focused_document_window->GetActionHistory()->redo_possible()) {
      redo_description = AssetEditor::last_focused_document_window->GetActionHistory()->redo_description();
    }
    ImGui::SetTooltip("Redo: %s", redo_description.c_str());
  }

  ImGui::PopStyleColor();

  ImGui::End();
}

void EditorWindowController::Save() {
  if (AssetEditor::last_focused_document_window != nullptr) {
    AssetEditor::last_focused_document_window->Save();
  }
}

void EditorWindowController::Undo() {
  if (AssetEditor::last_focused_document_window != nullptr) {
    AssetEditor::last_focused_document_window->GetActionHistory()->Undo();
  }
}

void EditorWindowController::Redo() {
  if (AssetEditor::last_focused_document_window != nullptr) {
    AssetEditor::last_focused_document_window->GetActionHistory()->Redo();
  }
}

}  // namespace ove