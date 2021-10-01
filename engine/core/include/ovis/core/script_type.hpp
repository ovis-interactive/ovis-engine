#pragma once

#include <any>
#include <limits>
#include <string_view>
#include <string>

namespace ovis {

using ScriptTypeId = size_t;
constexpr ScriptTypeId SCRIPT_TYPE_UNKNOWN = 0;

struct ScriptType {
  ScriptType(ScriptTypeId id, std::string_view name, bool reference_type)
      : id(id), name(name), reference_type(reference_type) {}

  ScriptTypeId id;
  std::string name;
  bool reference_type;
};

struct ScriptValueDefinition {
  ScriptTypeId type;
  std::string identifier;
};

struct ScriptValue {
  ScriptTypeId type;
  std::any value;
};

template <typename T>
concept Number = (std::is_integral_v<std::remove_reference_t<std::remove_cv_t<T>>> &&
                  !std::is_same_v<std::remove_reference_t<std::remove_cv_t<T>>, bool>) ||
                 std::is_floating_point_v<std::remove_reference_t<std::remove_cv_t<T>>>;


}
