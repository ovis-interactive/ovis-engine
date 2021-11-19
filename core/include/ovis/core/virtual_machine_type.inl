namespace ovis {
namespace vm {

inline Type::Type(Module* module, std::string_view name)
    : module_(module),
      name_(name),
      full_reference_(fmt::format("{}.{}", module->name(), name)),
      parent_(nullptr),
      serialize_function_(nullptr),
      deserialize_function_(nullptr) {}

inline Type::Type(Module* module, std::string_view name, Type* parent, ConversionFunction to_base,
                  ConversionFunction from_base)
    : Type(module, name) {
  parent_ = parent;
  from_base_ = from_base;
  to_base_ = to_base;
}

inline bool Type::IsDerivedFrom(safe_ptr<Type> type) const {
  if (type.get() == this) {
    return true;
  } else {
    return parent_ != nullptr && parent_->IsDerivedFrom(type);
  }
}

template <typename T>
inline bool Type::IsDerivedFrom() const {
  return IsDerivedFrom(Get<T>());
}

inline void Type::RegisterConstructorFunction(safe_ptr<Function> constructor) {
  if (!constructor) {
    return;
  }

  // Output must be the current type
  if (constructor->outputs().size() != 1 || constructor->outputs()[0].type != this) {
    return;
  }

  // Check if there is already a constructor with these parameters registered.
  for (const auto& function : constructor_functions_) {
    if (function->inputs().size() != constructor->inputs().size()) {
      continue;
    }
    bool arguments_different = false;
    for (auto i : IRange(function->inputs().size())) {
      if (function->inputs()[i].type != constructor->inputs()[i].type) {
        arguments_different = true;
        break;
      }
    }
    if (!arguments_different) {
      return;
    }
  }

  constructor_functions_.push_back(constructor);
}

template <typename... Args>
inline Value Type::Construct(Args&&... args) const {
  for (const auto& function : constructor_functions_) {
    if (function->IsCallableWithArguments<Args...>()) {
      return function->Call<Value>(std::forward<Args>(args)...);
    }
  }

  return Value::None();
}

template <typename T>
inline safe_ptr<Type> Type::Get() {
  if (auto it = type_associations.find(typeid(T)); it != type_associations.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

inline safe_ptr<Type> Type::Deserialize(const json& data) {
  std::string_view module_name;
  std::string_view type_name;

  if (data.is_string()) {
    std::string_view type_string = data.get_ref<const std::string&>();
    auto period_position = type_string.find('.');
    if (period_position == std::string_view::npos) {
      return nullptr;
    }
    module_name = type_string.substr(0, period_position);
    type_name = type_string.substr(period_position + 1);
  } else if (data.is_object()) {
    if (!data.contains("module")) {
      return nullptr;
    }
    const auto& module_json = data.at("module");
    if (!module_json.is_string()) {
      return nullptr;
    }
    module_name = module_json.get_ref<const std::string&>();
    if (!data.contains("name")) {
      return nullptr;
    }
    const auto& name_json = data.at("name");
    if (!name_json.is_string()) {
      return nullptr;
    }
    type_name = name_json.get_ref<const std::string&>();
  } else {
    return nullptr;
  }

  const safe_ptr<vm::Module> module = Module::Get(module_name);
  if (module == nullptr) {
    return nullptr;
  }
  return module->GetType(type_name);
}

inline void Type::RegisterProperty(std::string_view name, Type* type, Property::GetFunction getter,
                                   Property::SetFunction setter) {
  properties_.push_back({
      .type = safe_ptr(type),
      .name = std::string(name),
      .getter = getter,
      .setter = setter,
  });
}

namespace detail {

template <auto PROPERTY>
class PropertyCallbacks {};

template <typename T, typename PropertyType, PropertyType T::*PROPERTY>
struct PropertyCallbacks<PROPERTY> {
  static Value PropertyGetter(const ovis::vm::Value& object) { return Value::Create(object.Get<T>().*PROPERTY); }

  static void PropertySetter(ovis::vm::Value* object, const ovis::vm::Value& property_value) {
    assert(property_value.type() == Type::template Get<PropertyType>());
    object->Get<T>().*PROPERTY = property_value.Get<PropertyType>();
  }

  static void Register(Type* type, std::string_view name) {
    type->RegisterProperty(name, Type::Get<PropertyType>().get(), &PropertyGetter, &PropertySetter);
  }
};

template <auto GETTER>
struct PropertyGetter;

template <typename PropertyType, typename ContainingType, PropertyType (ContainingType::*GETTER)() const>
struct PropertyGetter<GETTER> {
  static safe_ptr<Type> property_type() { return Type::Get<PropertyType>(); }

  static safe_ptr<Type> containing_type() { return Type::Get<ContainingType>(); }

  // template <FunctionPointerType GETTER>
  static Value Get(const ovis::vm::Value& object) { return Value::Create((object.Get<ContainingType>().*GETTER)()); }
};

template <auto GETTER>
struct PropertySetter;

template <typename PropertyType, typename ContainingType, void (ContainingType::*SETTER)(PropertyType T)>
struct PropertySetter<SETTER> {
  static safe_ptr<Type> property_type() { return Type::Get<PropertyType>(); }

  static safe_ptr<Type> containing_type() { return Type::Get<ContainingType>(); }

  // template <FunctionPointerType GETTER>
  static void Set(ovis::vm::Value* object, const Value& property) {
    return (object->Get<ContainingType>().*SETTER)(property.Get<PropertyType>());
  }
};

}  // namespace detail

template <auto PROPERTY>
requires std::is_member_pointer_v<decltype(PROPERTY)>
inline void Type::RegisterProperty(std::string_view name) {
  detail::PropertyCallbacks<PROPERTY>::Register(this, name);
}

template <auto GETTER>
inline void Type::RegisterProperty(std::string_view name) {
  using GetterWrapper = detail::PropertyGetter<GETTER>;
  RegisterProperty(name, GetterWrapper::property_type(), &GetterWrapper::PropertyGetter, nullptr);
}

template <auto GETTER, auto SETTER>
inline void Type::RegisterProperty(std::string_view name) {
  using GetterWrapper = detail::PropertyGetter<GETTER>;
  using SetterWrapper = detail::PropertySetter<SETTER>;
  assert(GetterWrapper::property_type() == SetterWrapper::property_type());
  assert(GetterWrapper::containing_type() == SetterWrapper::containing_type());
  assert(GetterWrapper::containing_type() == this);
  RegisterProperty(name, GetterWrapper::property_type().get(), &GetterWrapper::Get, &SetterWrapper::Set);
}

inline const Type::Property* Type::GetProperty(std::string_view name) const {
  for (const auto& property : properties_) {
    if (property.name == name) {
      return &property;
    }
  }
  assert(false && "Property not found");
  return nullptr;
}

inline Value Type::CreateValue(const json& data) const {
  if (deserialize_function_) {
    return deserialize_function_(data);
  } else {
    return Value::None();
  }
}

inline json Type::Serialize() const {
  json type = json::object();
  type["name"] = std::string(name());
  if (parent()) {
    type["parent"] = std::string(parent()->full_reference());
  }

  json& properties = type["properties"] = json::object();
  for (const auto& property : this->properties()) {
    properties[property.name] = std::string(property.type->full_reference());
  }

  return type;
}

}  // namespace vm
}  // namespace ovis