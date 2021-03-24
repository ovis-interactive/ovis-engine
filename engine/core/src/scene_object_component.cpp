#include <ovis/utils/log.hpp>
#include <ovis/core/scene_object.hpp>
#include <ovis/core/scene_object_component.hpp>

namespace ovis {

void SceneObjectComponent::RegisterType(sol::table* module) {
  /// The base class of all components in the scene object.
  // @classmod ovis.core.SceneObjectComponent
  sol::usertype<SceneObjectComponent> scene_obect_component_type =
      module->new_usertype<SceneObjectComponent>("SceneObjectComponent", sol::no_constructor);

  /// The scene object the component is attached to.
  // @field[type=SceneObject] object
  scene_obect_component_type["object"] = sol::property(&SceneObjectComponent::scene_object);
}

}  // namespace ovis