#pragma once

#include <ovis/physics2d/physics2d_contact_event.hpp>

namespace ovis {

class Physics2DBeginContactEvent : public Physics2DContactEvent {
  OVIS_MAKE_DYNAMICALLY_LUA_REFERENCABLE();

 public:
  inline static const std::string_view TYPE = "Physics2DBeginContact";

  Physics2DBeginContactEvent(b2Contact* contact) : Physics2DContactEvent(TYPE, contact) {}

  static void RegisterType(sol::table* module);
};

}  // namespace ovis