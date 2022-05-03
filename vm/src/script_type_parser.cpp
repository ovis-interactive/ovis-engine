#include "ovis/vm/virtual_machine_instructions.hpp"
#include <ovis/vm/script_type_parser.hpp>

namespace ovis {

Result<ParseScriptTypeResult, ParseScriptErrors> ParseScriptType(VirtualMachine* virtual_machine,
                                                                 const json& type_definition) {
  TypeDescription description = {
    .memory_layout = {
      .alignment_in_bytes = ValueStorage::ALIGNMENT,
      .size_in_bytes = 0,
    }
  };
  ParseScriptErrors errors;
  std::vector<TypePropertyDescription> properties;
  if (const auto& name = type_definition["name"]; name.is_string()) {
    description.name = name;
  } else {
    errors.emplace_back(fmt::format("Invalid name"), "/name");
  }

  FunctionDescription construct_function = {
    .virtual_machine = virtual_machine,
    .inputs = {{ .name = "pointer", .type = virtual_machine->GetTypeId<void*>() }},
    .outputs = {},
    .definition = ScriptFunctionDefinition{},
  };
  ScriptFunctionDefinition& construct_function_definition = std::get<1>(construct_function.definition);

  for (const auto& [property_name, property_definition] : type_definition["properties"].items()) {
    const auto& property_type = virtual_machine->GetType(property_definition.at("type"));
    if (!property_type) {
      errors.emplace_back(
          fmt::format("Invalid type for property {}: {}", property_name, property_definition.at("type")),
          fmt::format("/properties/{}", property_name));
      continue;
    }
    if (property_type->alignment_in_bytes() > description.memory_layout.alignment_in_bytes) {
      description.memory_layout.alignment_in_bytes = property_type->alignment_in_bytes();
    }
    const std::size_t padding_bytes = property_type->alignment_in_bytes() -
                                      (description.memory_layout.size_in_bytes % property_type->alignment_in_bytes());
    description.memory_layout.size_in_bytes += padding_bytes;

    description.properties.push_back({
        .name = property_name,
        .type = property_type->id(),
        .access = TypePropertyDescription::PrimitiveAccess { .offset = description.memory_layout.size_in_bytes }
    });

    construct_function_definition.constants.push_back(Value::Create(virtual_machine, property_type->construct_function()->handle()));
    const std::size_t argument_offset =
        ExecutionContext::GetFunctionBaseOffset(0, 1) +
        (property_type->construct_function()->is_script_function() ? ExecutionContext::GetInputOffset(0, 0) : 0);

    if (property_type->construct_function()->is_script_function()) {
      construct_function_definition.instructions.push_back(Instruction::CreatePushExecutionState());
    }
    construct_function_definition.instructions.push_back(instructions::Push(1));
    construct_function_definition.instructions.push_back(
        instructions::CopyTrivialValue(argument_offset, ExecutionContext::GetInputOffset(0, 0)));
    construct_function_definition.instructions.push_back(Instruction::CreateOffsetAddress(argument_offset, description.memory_layout.size_in_bytes));
    construct_function_definition.instructions.push_back(Instruction::CreatePushTrivialConstant(0));
    construct_function_definition.instructions.push_back(Instruction::CallScriptFunction());

    description.memory_layout.size_in_bytes += property_type->size_in_bytes();
  }

  using ResultType = Result<ParseScriptTypeResult, ParseScriptErrors>;
  return errors.size() > 0 ? ResultType(errors) : ParseScriptTypeResult{ description };
}

}  // namespace ovis
