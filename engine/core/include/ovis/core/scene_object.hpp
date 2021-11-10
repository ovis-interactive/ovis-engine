#pragma once

#include <memory>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

#include <ovis/utils/class.hpp>
#include <ovis/utils/down_cast.hpp>
#include <ovis/utils/json.hpp>
#include <ovis/utils/range.hpp>
#include <ovis/utils/safe_pointer.hpp>
#include <ovis/utils/serialize.hpp>
#include <ovis/core/scene_object_component.hpp>
#include <ovis/core/virtual_machine.hpp>

namespace ovis {

class Scene;

class SceneObject : public Serializable, public SafelyReferenceable {
  MAKE_NON_COPYABLE(SceneObject);

 public:
  SceneObject(Scene* scene, std::string_view name, SceneObject* parent = nullptr);
  SceneObject(SceneObject&&) = default;
  ~SceneObject();

  SceneObject& operator=(SceneObject&&) = default;

  static bool IsValidName(std::string_view name);
  static std::pair<std::string_view, std::optional<unsigned int>> ParseName(std::string_view name);
  static std::string BuildPath(std::string_view object_name, SceneObject* parent) {
    return parent != nullptr ? fmt::format("{}/{}", parent->path(), object_name) : std::string(object_name);
  }

  inline Scene* scene() const { return scene_; }
  inline std::string_view name() const { return name_; }
  inline std::string_view path() const { return path_; }
  inline SceneObject* parent() const { return parent_.get(); }
  inline bool has_parent() const { return parent_ != nullptr; }

  // Setup the scene object with a specific template. This will completely reset the object and all
  // previously added components and child objects will be removed.
  bool SetupTemplate(std::string_view template_asset_id);

  SceneObject* CreateChildObject(std::string_view object_name);
  SceneObject* CreateChildObject(std::string_view object_name, const json& serialized_object);
  void DeleteChildObject(std::string_view object_name);
  void DeleteChildObject(SceneObject* object);
  void ClearChildObjects();
  SceneObject* GetChildObject(std::string_view object_name);
  bool HasChildren() const { return children_.size() > 0; }
  bool ContainsChildObject(std::string_view object_name);
  std::vector<safe_ptr<SceneObject>>& children() { return children_; }
  template <typename T>
  void ForEachChild(bool recursive, T&& functor);

  vm::Value AddComponent(safe_ptr<vm::Type> type);
  template <typename ComponentType> ComponentType* AddComponent();

  vm::Value GetComponent(safe_ptr<vm::Type> type);
  vm::Value GetComponent(safe_ptr<vm::Type> type) const;
  template <typename ComponentType> ComponentType* GetComponent();
  template <typename ComponentType> const ComponentType* GetComponent() const;

  // std::span<vm::Value> components() { return components_; }
  // std::span<const vm::Value> components() const { return components_; }

  bool HasComponent(safe_ptr<vm::Type> type) const;
  template <typename ComponentType> bool HasComponent() const;

  auto component_types() const { return TransformRange(components_, [](const auto& component) { return component.type; }); }

  bool RemoveComponent(safe_ptr<vm::Type> type);
  template <typename ComponentType> bool RemoveComponent();
  void ClearComponents();

  json Serialize() const override;
  bool Deserialize(const json& serialized_object) override;
  bool Update(const json& serialized_object) override;

  static void RegisterType(sol::table* module);

 private:
  Scene* scene_;  // No safe_ptr needed, scene is guaranteed to live longer and will (should?) not be moved
  safe_ptr<SceneObject> parent_;
  std::string path_;
  std::string name_;
  std::string template_;
  std::vector<safe_ptr<SceneObject>> children_;
  struct TypedComponent {
    safe_ptr<vm::Type> type;
    std::unique_ptr<SceneObjectComponent> pointer;
  };
  std::vector<TypedComponent> components_;

  std::vector<safe_ptr<SceneObject>>::const_iterator FindChild(std::string_view name) const;
  std::vector<safe_ptr<SceneObject>>::iterator FindChild(std::string_view name);
};

template <typename T>
void SceneObject::ForEachChild(bool recursive, T&& functor) {
  static_assert(std::is_invocable_v<T, SceneObject*>);
  for (const auto& child : children()) {
    functor(child.get_throw());
    if (recursive) {
      child->ForEachChild(true, functor);
    }
  }
}

}  // namespace ovis

namespace std {
template <>
struct hash<ovis::SceneObject> {
  size_t operator()(const ovis::SceneObject& object) const { return hash<std::string_view>()(object.path()); }
};
}  // namespace std

#include <ovis/core/scene_object.inl>
