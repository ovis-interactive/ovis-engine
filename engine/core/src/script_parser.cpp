#include <ovis/core/script_parser.hpp>

namespace ovis {

ScriptFunctionParser::ScriptFunctionParser(const json& function_definition) {
  if (!function_definition.is_object()) {
    errors.push_back({
        .message = "Function definition must be an object",
        .path = "/",
    });
  } else {
    inputs = ParseInputOutputDeclarations(function_definition["inputs"]);
    outputs = ParseInputOutputDeclarations(function_definition["outputs"]);
    ParseActions(function_definition["actions"], "/actions");
  }
}

std::vector<Function::ValueDeclaration> ScriptFunctionParser::ParseInputOutputDeclarations(const json& value_declarations) { return {}; }

void ScriptFunctionParser::ParseActions(const json& actions, std::string path) {
  if (!actions.is_array()) {
    errors.push_back({
        .message = "Actions must be an array.",
        .path = path,
    });
  } else {
    for (std::size_t i = 0, size = actions.size(); i < size; ++i) {
      ParseAction(actions[i], fmt::format("{}/{}", path, i));
    }
  }
}

void ScriptFunctionParser::ParseAction(const json& action, std::string path) {
  if (!action.is_object()) {
    errors.push_back({
        .message = "Action must be an object.",
        .path = path,
    });
  } else if (const auto type_it = action.find("type"); type_it == action.end()) {
    errors.push_back({
        .message = "Action must contain key 'type'.",
        .path = path,
    });
  } else if (!type_it->is_string()) {
    errors.push_back({
        .message = "Key 'type' must be a string.",
        .path = path,
    });
  } else if (*type_it == "function_call") {
    ParseFunctionCallAction(action, path);
  } else if (*type_it == "if") {
    ParseIf(action, path);
  } else if (*type_it == "while") {
    ParseWhile(action, path);
  } else {
    errors.push_back({
        .message = fmt::format("Unknown value for action type: '{}'", *type_it),
        .path = path,
    });
  }
}

void ScriptFunctionParser::ParseFunctionCallAction(const json& action, std::string path) {
  assert(action.is_object()); // Should have already been checked before
  if (const auto function = action.find("function"); function == action.end()) {
    errors.push_back({
        .message = fmt::format("Function call does not include key 'function'."),
        .path = path,
    });
  } else if (!function->is_object()) {
    errors.push_back({
        .message = fmt::format("Function key must be an object."),
        .path = path,
    });
  } else if (const auto function_name = function->find("name"); function_name == function->end()) {
    errors.push_back({
        .message = fmt::format("Function must contain key 'name'."),
        .path = path,
    });
  } else if (!function_name->is_string()) {
    errors.push_back({
        .message = fmt::format("Function key 'module' must be of type string."),
        .path = path,
    });
  } else if (const auto function_module = function->find("module"); function_module == function->end()) {
    errors.push_back({
        .message = fmt::format("Function must contain key 'module'."),
        .path = path,
    });
  } else if (!function_module->is_string()) {
    errors.push_back({
        .message = fmt::format("Function key 'module' must be of type string."),
        .path = path,
    });
  } else if (safe_ptr<Module> module = Module::Get(static_cast<std::string>(*function_module)); module == nullptr) {
    errors.push_back({
        .message = fmt::format("Module {} not found.", *function_module),
        .path = path,
    });
  } else if (safe_ptr<Function> function_reflection = module->GetFunction(static_cast<std::string>(*function_name)); function_reflection == nullptr) {
    errors.push_back({
        .message = fmt::format("Function {} not found in module {}.", *function_name, *function_module),
        .path = path,
    });
  } else if (const auto& outputs = action.find("outputs"); outputs == action.end()) {
    errors.push_back({
        .message = fmt::format("Function call action must contain key 'outputs'."),
        .path = path,
    });
  } else if (!outputs->is_object()) {
    errors.push_back({
        .message = fmt::format("Function key 'outputs' must be an object."),
        .path = path,
    });
  } else if (const auto& inputs = action.find("inputs"); inputs == action.end()) {
    errors.push_back({
        .message = fmt::format("Function call action must contain key 'inputs'."),
        .path = path,
    });
  } else if (!inputs->is_object()) {
    errors.push_back({
        .message = fmt::format("Function key 'inputs' must be an object."),
        .path = path,
    });
  } else {
    std::vector<std::optional<int>> output_positions(function_reflection->outputs().size());

    for (const auto& [output_name, local_variable] : outputs->items()) {
      if (!local_variable.is_string()) {
        errors.push_back({
            .message = fmt::format("Local variable name of output '{}' must be a string.", output_name),
            .path = path,
        });
      } else if (const auto& output_declaration = function_reflection->GetOutput(output_name); !output_declaration.has_value()) {
        errors.push_back({
            .message = fmt::format("Output '{}' not present for function.", output_name, function_reflection->name()),
            .path = path,
        });
      } else if (const auto output_variable_position = GetOutputVariablePosition(static_cast<std::string>(local_variable), output_declaration->type, path); output_variable_position.has_value()) {
        errors.push_back({
            .message = fmt::format("Mismatched types."),
            .path = path,
        });
      } else {
        assert(function_reflection->GetOutputIndex(output_name).has_value());
        output_positions[*function_reflection->GetOutputIndex(output_name)] = *output_variable_position;
      }
    }

    for (const auto& input_declaration : function_reflection->inputs()) {
      if (const auto& input_definition = inputs->find(input_declaration.name); input_definition == inputs->end()) {
        errors.push_back({
            .message = fmt::format("Missing input: '{}'.", input_declaration.name),
            .path = path,
        });
      } else {
        ParsePush(*input_definition, input_declaration.type);
      }
    }

    for (const auto& output : function_reflection->outputs()) {
      PushNone(path);
    }

    instructions.push_back(script_instructions::FunctionCall {
        .function_pointer = function_reflection->pointer(),
        .input_count = static_cast<int>(function_reflection->inputs().size()),
        .output_count = static_cast<int>(function_reflection->outputs().size()),
    });
    debug_info.instruction_info.push_back({
      .scope = current_scope_index,
      .action = path,
    });

    for (const auto& output_position : IndexRange(output_positions)) {
      if (output_position->has_value()) {
        instructions.push_back(script_instructions::AssignStackValue {
            .source_position = static_cast<int16_t>(static_cast<int16_t>(output_position.index()) - static_cast<int16_t>(output_positions.size())),
            .destination_position = static_cast<int16_t>(**output_position),
        });
        debug_info.instruction_info.push_back({
          .scope = current_scope_index,
          .action = path,
        });
      }
    }

    instructions.push_back(script_instructions::Pop {
        .count = static_cast<int>(function_reflection->inputs().size() + function_reflection->outputs().size())
    });
    debug_info.instruction_info.push_back({
      .scope = current_scope_index,
      .action = path,
    });
  }
}

void ScriptFunctionParser::ParseIf(const json& action, std::string path) {
  assert(action.is_object()); // Should have already been checked before
  if (const auto condition = action.find("condition"); condition == action.end()) {
    errors.push_back({
        .message = fmt::format("If requires key 'condition'."),
        .path = path,
    });
  } else if (const auto actions = action.find("actions"); actions == action.end()) {
    errors.push_back({
        .message = fmt::format("If requires key 'actions'."),
        .path = path,
    });
  } else {
    ParsePush(*condition, Type::Get<bool>());

    const std::size_t instruction_count_before = instructions.size();
    instructions.push_back(script_instructions::JumpIfFalse{});
    debug_info.instruction_info.push_back({
      .scope = current_scope_index,
      .action = path,
    });
    ParseActions(*actions, fmt::format("{}/actions", path));
    const std::size_t instruction_count_after = instructions.size();

    assert(instruction_count_after >= instruction_count_before);
    std::get<script_instructions::JumpIfFalse>(instructions[instruction_count_before]).instruction_offset =
        instruction_count_after - instruction_count_before;
  }
}

void ScriptFunctionParser::ParseWhile(const json& action, std::string path) {
  assert(action.is_object()); // Should have already been checked before
  if (const auto condition = action.find("condition"); condition == action.end()) {
    errors.push_back({
        .message = fmt::format("If requires key 'condition'."),
        .path = path,
    });
  } else if (const auto actions = action.find("actions"); actions == action.end()) {
    errors.push_back({
        .message = fmt::format("If requires key 'actions'."),
        .path = path,
    });
  } else {
    ParsePush(*condition, Type::Get<bool>());

    const int instruction_count_before = instructions.size();
    instructions.push_back(script_instructions::JumpIfFalse{});
    debug_info.instruction_info.push_back({
      .scope = current_scope_index,
      .action = path,
    });
    ParseActions(*actions, fmt::format("{}/actions", path));
    instructions.push_back(script_instructions::Jump {
        .instruction_offset = static_cast<std::ptrdiff_t>(instruction_count_before) - static_cast<std::ptrdiff_t>(instructions.size()),
    });
    debug_info.instruction_info.push_back({
      .scope = current_scope_index,
      .action = path,
    });

    std::get<script_instructions::JumpIfFalse>(instructions[instruction_count_before]).instruction_offset =
        instructions.size() - instruction_count_before;
  }
}

void ScriptFunctionParser::PushNone(const std::string& path) {
  instructions.push_back(script_instructions::PushConstant {
      .value = Value{}
  });
  debug_info.instruction_info.push_back({
      .scope = current_scope_index,
      .action = path,
  });
}

void ScriptFunctionParser::ParsePush(const json& value_definiion, safe_ptr<Type> required_type) {}

std::optional<int> ScriptFunctionParser::GetOutputVariablePosition(std::string_view name, safe_ptr<Type> type, const std::string& path) {
  const auto local_variable = GetLocalVariable(name);
  if (local_variable.has_value()) {
    if (local_variable->declaration.type == type) {
      return local_variable->position;
    } else {
      return {};
    }
  } else {
    PushNone(path);
    const int position = current_scope().position_offset + current_scope().variables.size();
    current_scope().variables.push_back({
        .declaration = {
          .name = std::string(name),
          .type = type,
        },
        .position = position,
    });
    return position;
  }
}

std::optional<ScriptFunction::DebugInfo::Scope::Variable> ScriptFunctionParser::GetLocalVariable(std::string_view name) {
  std::size_t scope_index = current_scope_index;
  do {
    const ScriptFunction::DebugInfo::Scope& scope = debug_info.scope_info[scope_index];
    for (const auto& variable : scope.variables) {
      if (variable.declaration.name == name) {
        return variable;
      }
    }
    scope_index = scope.parent_scope;
  } while (scope_index != std::numeric_limits<std::size_t>::max());

  return {};
}

}