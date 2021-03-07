#include "editor_asset_library.hpp"
#include "editor_module.hpp"
#include "editor_window.hpp"
#include "global.hpp"
#include "imgui_extensions/input_json.hpp"
#include "imgui_extensions/input_math.hpp"
#include <cstring>

#include <emscripten.h>
#include <emscripten/val.h>
#include <ovis/base/base_module.hpp>

#include <ovis/core/asset_library.hpp>
#include <ovis/core/log.hpp>
#include <ovis/engine/engine.hpp>
#include <ovis/rendering2d/rendering2d_module.hpp>
#include <ovis/physics2d/physics2d_module.hpp>

// Usage: ovis-editor backend_url project_id authentication_token
int main(int argc, char* argv[]) {
  using namespace ovis;
  using namespace ovis::editor;

  ImGui::SetCustomJsonFunction("math#/$defs/vector2", &ImGui::InputVector2);
  ImGui::SetCustomJsonFunction("math#/$defs/vector3", &ImGui::InputVector3);
  ImGui::SetCustomJsonFunction("math#/$defs/vector4", &ImGui::InputVector4);
  ImGui::SetCustomJsonFunction("math#/$defs/color", &ImGui::InputColor);

  try {
    Log::AddListener(ConsoleLogger);

    if (argc != 4) {
      LogE("Invalid number of arguments to editor");
      return -1;
    }

    backend_url = argv[1];
    project_id = argv[2];
    authentication_token = argv[3];

    Init();
    SetEngineAssetsDirectory("/ovis_assets");
    CreateApplicationAssetLibrary<EditorAssetLibrary>("/assets/");
    LoadModule<BaseModule>();
    LoadModule<Rendering2DModule>();
    LoadModule<Physics2DModule>();
    LoadModule<EditorModule>();

    EditorWindow editor_window;

    Run();

    LogI("Quitting editor...");

    Quit();
  } catch (const std::exception& error) {
    LogE("An unhandled exception occured: {}", error.what());
  } catch (...) {
    LogE("An unhandled exception occured");
  }
  return 0;
}