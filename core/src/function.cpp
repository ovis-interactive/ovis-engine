#include <ovis/core/function.hpp>
#include <ovis/core/module.hpp>

namespace ovis {

Function::Function(std::string_view name, NativeFunction* function_pointer, std::vector<ValueDeclaration> inputs,
                   std::vector<ValueDeclaration> outputs)
    : name_(name), inputs_(inputs), outputs_(outputs) {
  handle_.native_function = function_pointer;
  // Check that the two least significant bits are zero. I.e., the function pointer is aligned to a four byte boundary.
  assert(handle_.is_script_function == 0);
  assert(handle_.zero == 0);
}

std::shared_ptr<Function> Function::MakeNative(NativeFunction* function_pointer, std::vector<ValueDeclaration> inputs,
                                               std::vector<ValueDeclaration> outputs) {
  return std::shared_ptr<Function>(new Function("", function_pointer, inputs, outputs));
}

std::shared_ptr<Function> Function::Deserialize(const json& data) {
  if (!data.contains("module")) {
    return nullptr;
  }
  const auto& module_json = data.at("module");
  if (!module_json.is_string()) {
    return nullptr;
  }
  const std::shared_ptr<Module> module = Module::Get(module_json.get_ref<const std::string&>());
  if (module == nullptr) {
    return nullptr;
  }
 if (!data.contains("name")) {
    return nullptr;
  }
  const auto& name_json = data.at("name");
  if (!name_json.is_string()) {
    return nullptr;
  }
  return module->GetFunction(name_json.get_ref<const std::string&>());
}

}  // namespace ovis