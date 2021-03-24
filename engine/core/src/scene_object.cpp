#include <SDL2/SDL_assert.h>

#include <ovis/utils/log.hpp>
#include <ovis/core/scene.hpp>
#include <ovis/core/scene_object.hpp>

namespace ovis {

SceneObject::SceneObject(Scene* scene, const std::string& name) : scene_(scene), name_(name) {
  SDL_assert(scene_ != nullptr);
}

SceneObject::~SceneObject() {}

SceneObjectComponent* SceneObject::AddComponent(const std::string& component_id) {
  if (HasComponent(component_id)) {
    LogE("Object '{}' already has the component '{}'.", name_, component_id);
    return nullptr;
  } else {
    std::optional<std::unique_ptr<SceneObjectComponent>> component = SceneObjectComponent::Create(component_id);
    if (!component.has_value()) {
      LogE("Component '{}' not registered or failed to create", component_id);
      return nullptr;
    } else {
      SDL_assert(*component != nullptr);
      (*component)->scene_object_ = this;
      return components_.insert(std::make_pair(component_id, std::move(*component))).first->second.get();
    }
  }
}

bool SceneObject::HasComponent(const std::string& component_id) const {
  return components_.count(component_id) != 0;
}

void SceneObject::GetComponentIds(std::vector<std::string>* component_ids) const {
  SDL_assert(component_ids != nullptr);

  component_ids->clear();
  component_ids->reserve(components_.size());
  for (const auto& component : components_) {
    component_ids->push_back(component.first);
  }
}

void SceneObject::RemoveComponent(const std::string& component_id) {
  if (components_.erase(component_id) == 0) {
    LogE("Failed to erase component '{}' from '{}': component does not exist.");
  }
}

json SceneObject::Serialize() const {
  json serialized_object = json::object();
  auto& components = serialized_object["components"] = json::object();
  for (const auto& component : components_) {
    components[component.first] = component.second->Serialize();
  }
  return serialized_object;
}

bool SceneObject::Deserialize(const json& serialized_object) {
  if (!serialized_object.is_object()) {
    return false;
  }
  if (serialized_object.contains("components")) {
    if (!serialized_object["components"].is_object()) {
      return false;
    }
    for (const auto& component : serialized_object["components"].items()) {
      if (!SceneObjectComponent::IsRegistered(component.key())) {
        return false;
      }
      AddComponent(component.key())->Deserialize(component.value());
    }
  } else {
    LogV("SceneObject deserialization: no 'components' property available!");
  }
  return true;
}

void SceneObject::RegisterType(sol::table* module) {
  /// Represents an object in a scene
  // @classmod ovis.core.SceneObject
  // @testinginclude <ovis/core/scene.hpp>
  // @cppsetup ovis::Scene scene;
  // @cppsetup ovis::lua["some_scene"] = &scene;
  // @usage local core = require "ovis.core"
  // local some_object = some_scene:add_object("Some Object")
  sol::usertype<SceneObject> scene_object_type = module->new_usertype<SceneObject>("SceneObject", sol::no_constructor);

  /// The name of the object.
  // Names of objects with the same parent must be unique.
  // @see 02-scene-structure.md
  // @usage assert(some_object.name == "Some Object")
  scene_object_type["name"] = sol::property(&SceneObject::name);

  /// Adds a component to the object.
  // @function add_component
  // @param id The id of the component.
  // @return The newly added component
  // @usage local transform = some_object:add_component("Transform")
  // assert(transform ~= nil)
  scene_object_type["add_component"] =
      static_cast<SceneObjectComponent* (SceneObject::*)(const std::string&)>(&SceneObject::AddComponent);

  /// Checks whether a component is attached to an object.
  // @function has_component
  // @param id The id of the component.
  // @treturn bool
  // @usage assert(not some_object:has_component("Transform"))
  // some_object:add_component("Transform")
  // assert(some_object:has_component("Transform"))
  scene_object_type["has_component"] = &SceneObject::HasComponent;

  /// Removes a component from the object.
  // @function remove_component
  // @param id The id of the component to remove.
  // @usage assert(not some_object:has_component("Transform"))
  // some_object:add_component("Transform")
  // assert(some_object:has_component("Transform"))
  // some_object:remove_component("Transform")
  // assert(not some_object:has_component("Transform"))
  scene_object_type["remove_component"] = &SceneObject::RemoveComponent;

  /// Returns a component.
  // @function get_component
  // @param id The id of the component.
  // @usage local transform = some_object:get_component("Transform")
  // assert(transform == nil)
  // some_object:add_component("Transform")
  // transform = some_object:get_component("Transform")
  // assert(transform ~= nil)
  scene_object_type["get_component"] = [](SceneObject* object, const std::string& component_id) {
    SceneObjectComponent* component = object->GetComponent(component_id);
    return component ? component->GetValue() : nullptr;
  };
}

}  // namespace ovis