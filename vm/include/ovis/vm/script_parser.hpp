#pragma once

#include <unordered_map>

#include "ovis/utils/not_null.hpp"
#include "ovis/vm/script_function_parser.hpp"
#include "ovis/vm/script_type_parser.hpp"

namespace ovis {

struct ParseScriptResult {
  std::vector<ParseScriptFunctionResult> functions;
  std::vector<ParseScriptTypeResult> types;
};

Result<ParseScriptResult, ParseScriptErrors> ParseScript(VirtualMachine* virtual_machine, const json& script);

class ScriptParser {
 public:
  ScriptParser(NotNull<VirtualMachine*> virtual_machine, std::string_view module_name);

  void AddScript(json script_definition, std::string_view name);

  const ParseScriptErrors& errors() const { return errors_; }

 private:
  NotNull<VirtualMachine*> virtual_machine_;
  std::shared_ptr<Module> module_;
  ParseScriptErrors errors_;

  struct TypeDefinition {
    json definition;
    std::string script_name;
    TypeId type_id;
  };
  std::unordered_map<std::string, TypeDefinition> type_definitions_;

  struct FunctionDefinition {
    json definition;
    std::string script_name;
    std::shared_ptr<Function> function;
  };
  std::unordered_map<std::string, FunctionDefinition> function_definitions_;
};

}  // namespace ovis
