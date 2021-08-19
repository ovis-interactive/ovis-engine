#include <imgui.h>

namespace ImGui {

bool HorizontalSplitter(const char* label, float* top_height, float* bottom_height, float top_weight = 1.0f,
                        float bottom_weight = 1.0f, ImVec2 size = ImVec2(0.0f, 0.0f), float top_min_height = 4.0f,
                        float bottom_min_height = 4.0f, float thickness = 4.0f);

bool VerticalSplitter(const char* label, float* left_width, float* right_width, float left_weight = 1.0f,
                      float right_weight = 1.0f, ImVec2 size = ImVec2(0.0f, 0.0f), float left_min_width = 4.0f,
                      float right_min_width = 4.0f, float thickness = 4.0f);
}