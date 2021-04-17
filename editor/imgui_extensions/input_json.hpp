#pragma once

#include <ovis/utils/json.hpp>

enum ImGuiInputJsonFlags {

  ImGuiInputJsonFlags_IgnoreEnclosingObject = 1

};

namespace ImGui {

bool InputJson(const char* label, ovis::json* value, const ovis::json& schema, bool* keep = nullptr, int flags = 0);

using InputJsonFunction = bool (*)(const char*, ovis::json*, const ovis::json&, int);
void SetCustomJsonFunction(const std::string& schema_reference, InputJsonFunction function);

}  // namespace ImGui