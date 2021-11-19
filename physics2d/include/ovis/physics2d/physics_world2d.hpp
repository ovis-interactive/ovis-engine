#pragma once

#include <box2d/b2_world.h>
#include <box2d/b2_world_callbacks.h>

#include <ovis/core/scene_controller.hpp>

namespace ovis {

class RigidBody2D;
class RigidBody2DFixture;

class PhysicsWorld2D : public SceneController, public b2ContactListener {
  friend class Physics2DDebugLayer;
  friend class RigidBody2D;
  friend class RigidBody2DFixture;

 public:
  static constexpr std::string_view Name() { return "PhysicsWorld2D"; }

  PhysicsWorld2D();
  ~PhysicsWorld2D();

  void Update(std::chrono::microseconds delta_time) override;

  const json* GetSchema() const override;
  json Serialize() const override;
  bool Deserialize(const json& data) override;

  // b2ContactListener methods
  void BeginContact(b2Contact* contact) override;
  void EndContact(b2Contact* contact) override;
  void PreSolve(b2Contact* contact, const b2Manifold* old_manifold) override;
  void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

 private:
  b2World world_;
  float update_rate_ = 50.0f;
  int velocity_iterations_ = 6;
  int position_iterations_ = 6;
  std::chrono::microseconds accumulated_time_;

  std::vector<RigidBody2D*> bodies_to_create_;
  std::vector<b2Body*> bodies_to_destroy_;

  std::vector<RigidBody2DFixture*> fixtures_to_create_;
  std::vector<b2Fixture*> fixtures_to_destroy_;

  static const json SCHEMA;
};

}  // namespace ovis