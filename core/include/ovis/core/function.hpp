#pragma once

#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <ovis/utils/result.hpp>
#include <ovis/core/virtual_machine.hpp>
#include <ovis/core/function_handle.hpp>

namespace ovis {

template <typename... T> struct FunctionResult { using type = std::tuple<T...>; };
template <typename T> struct FunctionResult <T> { using type = T; };
template <> struct FunctionResult <> { using type = void; };
template <typename... T> using FunctionResultType = typename FunctionResult<T...>::type;


// A function can either be a native (C++) function or script function.
class Function : public std::enable_shared_from_this<Function> {
  friend class Module;
  friend class Type;

 public:
  struct ValueDeclaration {
    std::string name;
    std::weak_ptr<Type> type;
  };

  std::string_view name() const { return name_; }
  std::string_view text() const { return text_; }

  // Returns the native function pointer. It is only valid to call this function if the function is actually a native
  // function. Can be checked via is_native_function().
  NativeFunction* native_function_pointer() const;
  bool is_native_function() const { return !handle_.is_script_function; }

  // Returns the offset into the instruction storage of the virtual machine. It is only valid to call this function if
  // the function is actually a script function. You can call is_script_function() to check this.
  std::uintptr_t instruction_offset() const;
  bool is_script_function() const { return handle_.is_script_function; }

  // Returns the handle of the function
  FunctionHandle handle() const { return handle_; }

  std::span<const ValueDeclaration> inputs() const { return inputs_; }
  std::optional<std::size_t> GetInputIndex(std::string_view input_name) const;
  std::optional<ValueDeclaration> GetInput(std::string_view input_name) const;
  std::optional<ValueDeclaration> GetInput(std::size_t input_index) const;

  std::span<const ValueDeclaration> outputs() const { return outputs_; }
  std::optional<std::size_t> GetOutputIndex(std::string_view output_name) const;
  std::optional<ValueDeclaration> GetOutput(std::size_t output_index) const;
  std::optional<ValueDeclaration> GetOutput(std::string_view output_name) const;

  // TODO: implement this function
  template <typename... InputTypes> bool IsCallableWithArguments() const { return true; }

  // template <typename... OutputTypes, typename... InputsTypes>
  // FunctionResultType<OutputTypes...> Call(InputsTypes&&... inputs);
  // template <typename... OutputTypes, typename... InputsTypes>
  // FunctionResultType<OutputTypes...> Call(ExecutionContext* context, InputsTypes&&... inputs);
  template <typename OutputType, typename... InputsTypes> Result<OutputType> Call(InputsTypes&&... inputs) const;

  json Serialize() const;
  static std::shared_ptr<Function> Deserialize(const json& data);

  static std::shared_ptr<Function> MakeNative(NativeFunction* function_pointer, std::vector<ValueDeclaration> inputs,
                                              std::vector<ValueDeclaration> outputs);

 private:
  Function(std::string_view name, NativeFunction* function_pointer, std::vector<ValueDeclaration> inputs,
           std::vector<ValueDeclaration> outputs);

  std::string name_;
  std::string text_;
  FunctionHandle handle_; // This handle has always the unused bit set to 0.
  std::vector<ValueDeclaration> inputs_;
  std::vector<ValueDeclaration> outputs_;

  auto FindInput(std::string_view name) const {
    return std::find_if(inputs().begin(), inputs().end(), [name](const auto& value) { return value.name == name; });
  }
  auto FindOutput(std::string_view name) const {
    return std::find_if(outputs().begin(), outputs().end(), [name](const auto& value) { return value.name == name; });
  }
};

inline std::optional<std::size_t> Function::GetInputIndex(std::string_view input_name) const {
  const auto input = FindInput(input_name);
  if (input == inputs().end()) {
    return std::nullopt;
  } else {
    return std::distance(inputs().begin(), input);
  }
}

inline std::optional<Function::ValueDeclaration> Function::GetInput(std::string_view input_name) const {
  const auto input = FindInput(input_name);
  if (input == inputs().end()) {
    return std::nullopt;
  } else {
    return *input;
  }
}

inline std::optional<Function::ValueDeclaration> Function::GetInput(std::size_t input_index) const {
  assert(input_index < inputs().size());
  return *(inputs().begin() + input_index);
}

inline std::optional<std::size_t> Function::GetOutputIndex(std::string_view output_name) const {
  const auto output = FindOutput(output_name);
  if (output == outputs().end()) {
    return std::nullopt;
  } else {
    return std::distance(outputs().begin(), output);
  }
}

inline std::optional<Function::ValueDeclaration> Function::GetOutput(std::string_view output_name) const {
  const auto output = FindOutput(output_name);
  if (output == outputs().end()) {
    return std::nullopt;
  } else {
    return *output;
  }
}

inline std::optional<Function::ValueDeclaration> Function::GetOutput(std::size_t output_index) const {
  assert(output_index < outputs().size());
  return *(outputs().begin() + output_index);
}

template <typename OutputType = void, typename... InputTypes>
inline Result<OutputType> Function::Call(InputTypes&&... inputs) const {
  assert(IsCallableWithArguments<InputTypes...>());
  return ExecutionContext::global_context()->Call<OutputType>(handle_, std::forward<InputTypes>(inputs)...);
}

inline NativeFunction* Function::native_function_pointer() const {
  assert(!handle_.is_script_function);
  assert(handle_.zero == 0);
  return handle_.native_function;
}

inline std::uintptr_t Function::instruction_offset() const {
  assert(handle_.is_script_function);
  return handle_.instruction_offset;
}

}  // namespace ovis
