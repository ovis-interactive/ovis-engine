#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

#include "ovis/utils/versioned_index.hpp"
#include "ovis/core/vm_bindings.hpp"

namespace ovis {

class Scene;

using EntityIdVersionedIndex = VersionedIndex<uint32_t, 15, 16, 1>;

struct EntityId : EntityIdVersionedIndex {
  template <typename... T>
  requires(std::is_constructible_v<EntityIdVersionedIndex, T...>)
  constexpr EntityId(T&&... arguments) : VersionedIndex(std::forward<T>(arguments)...) {}

  bool is_active() const {
    return flags != 0;
  }

  constexpr static EntityId CreateInactive(uint32_t index) {
    return EntityId(index, 0, 0);
  }
};

struct Entity {
  EntityId id;
  EntityId parent_id;
  EntityId first_children_id;
  EntityId previous_sibling_id;
  EntityId next_sibling_id;
  std::string name;

  struct Siblings;
  struct SiblingIterator;
  struct ChildrenIterator;

  bool is_active() const {
    return id.is_active();
  }

  void activate() {
    assert(!is_active());
    id.flags = 1;
  }

  void deactivate() {
    assert(is_active());
    id.flags = 0;
  }

  bool has_parent() const {
    return parent_id != id;
  }

  bool has_children() const {
    return first_children_id != id;
  }

  bool has_siblings() const {
    return next_sibling_id != id;
  }

  Siblings siblings(Scene* scene) const;

  static bool IsValidName(std::string_view name);
  static std::pair<std::string_view, std::optional<unsigned int>> ParseName(std::string_view name);

  OVIS_VM_DECLARE_TYPE_BINDING();
};

#if OVIS_EMSCRIPTEN
  static_assert(sizeof(Entity) == 32);
#endif

struct Entity::SiblingIterator {
  friend bool operator==(const Entity::SiblingIterator& lhs, const Entity::SiblingIterator& rhs);
  friend bool operator!=(const Entity::SiblingIterator& lhs, const Entity::SiblingIterator& rhs);

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = Entity;
  using pointer = Entity*;
  using reference = Entity&;

  Entity& operator*() const;
  Entity* operator->();
  SiblingIterator& operator++();
  SiblingIterator operator++(int);
  SiblingIterator& operator--();
  SiblingIterator operator--(int);

  Scene* scene;
  EntityId current_sibling_id;
};

struct Entity::Siblings {
  Scene* scene;
  EntityId entity_id;
};

bool operator==(const Entity::SiblingIterator& lhs, const Entity::SiblingIterator& rhs);
bool operator!=(const Entity::SiblingIterator& lhs, const Entity::SiblingIterator& rhs);

Entity::SiblingIterator begin(const Entity::Siblings& siblings);
Entity::SiblingIterator end(const Entity::Siblings& siblings);

static_assert(std::bidirectional_iterator<Entity::SiblingIterator>);

// static_assert(std::forward_iterator<Entity::SiblingIterator>);

}  // namespace ovis
