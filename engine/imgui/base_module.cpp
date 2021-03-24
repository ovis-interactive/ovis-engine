#include <ovis/base/base_module.hpp>
#include <ovis/base/camera_component.hpp>
#include <ovis/base/clear_render_pass.hpp>
#include <ovis/base/imgui_render_pass.hpp>
#include <ovis/base/imgui_scene_controller.hpp>
#include <ovis/base/transform_component.hpp>

#include <ovis/core/platform.hpp>
#include <ovis/engine/lua.hpp>

namespace ovis {

BaseModule::BaseModule() : Module("BaseModule") {
  IMGUI_CHECKVERSION();
  context_ = ImGui::CreateContext();

#if !OVIS_EMSCRIPTEN
  ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
#endif
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui::GetIO().IniFilename = "/user/imgui.ini";

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup back-end capabilities flags
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigMacOSXBehaviors = GetPlatform() == Platform::MACOS;
  //   io.BackendFlags |=
  //       ImGuiBackendFlags_HasMouseCursors;  // We can honor GetMouseCursor()
  //                                           // values (optional)
  //   io.BackendFlags |=
  //       ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos
  //                                          // requests (optional, rarely
  //                                          used)

  // Keyboard mapping. ImGui will use those indices to peek into the
  // io.KeysDown[] array.
  io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
  io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
  io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
  io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
  io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
  io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
  io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
  io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
  io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
  io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
  io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
  io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
  io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
  io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
  io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
  io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
  io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

  RegisterRenderPass("Clear", [this](Viewport*) { return std::make_unique<ClearRenderPass>(); });
  RegisterRenderPass("ImGui", [this](Viewport*) { return std::make_unique<ImGuiRenderPass>(context_); });
  RegisterSceneController("ImGui", [this](Scene*) { return std::make_unique<ImGuiSceneController>(context_); });
  RegisterSceneObjectComponent<TransformComponent>("Transform",
                                                   [](SceneObject*) { return std::make_unique<TransformComponent>(); });
  RegisterSceneObjectComponent<CameraComponent>("Camera",
                                                [](SceneObject*) { return std::make_unique<CameraComponent>(); });

  sol::usertype<TransformComponent> transform_component_type = Lua::state.new_usertype<TransformComponent>("Transform");
  transform_component_type["position"] =
      sol::property([](const TransformComponent* transform_component) { return transform_component->position(); },
                    [](TransformComponent* transform_component, Vector3 position) {
                      return transform_component->SetPosition(position);
                    });
  transform_component_type["transform_direction"] = &TransformComponent::TransformDirection;
  transform_component_type["rotate"] =
      static_cast<void (TransformComponent::*)(Vector3, float)>(&TransformComponent::Rotate);
  transform_component_type["move"] = &TransformComponent::Move;
}

BaseModule::~BaseModule() {
  ImGui::DestroyContext(context_);
}

}  // namespace ovis