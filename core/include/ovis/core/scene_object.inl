#pragma once

namespace ovis {

template <typename ComponentType>
inline ComponentType* SceneObject::AddComponent() {
  return AddComponent(vm::Type::Get<ComponentType>()).template Get<ComponentType*>();
}

template <typename ComponentType>
inline ComponentType* SceneObject::GetComponent() {
  return GetComponent(vm::Type::Get<ComponentType>()).template Get<ComponentType*>();
}

template <typename ComponentType>
inline const ComponentType* SceneObject::GetComponent() const {
  return GetComponent(vm::Type::Get<ComponentType>()).template Get<ComponentType*>();
}

template <typename ComponentType>
inline bool SceneObject::HasComponent() const {
  return HasComponent(vm::Type::Get<ComponentType>());
}

template <typename ComponentType>
inline bool SceneObject::RemoveComponent() { 
  return RemoveComponent(vm::Type::Get<ComponentType>());
}

}