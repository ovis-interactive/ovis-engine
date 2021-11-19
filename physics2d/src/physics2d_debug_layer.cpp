

namespace ovis {

// namespace {

// Vector3 ConvertVector(const b2Vec2& vector) {
//   return {vector.x, vector.y, 0.0f};
// }

// Color ConvertColor(const b2Color& color) {
//   return {color.r, color.g, color.b, color.a};
// }

// Color GetSolidColor(const b2Color& color) {
//   return {color.r * 0.5f, color.g * 0.5f, color.b * 0.5f, 0.5f};
// }

// }  // namespace

// Physics2DDebugLayer::Physics2DDebugLayer() : PrimitiveRenderer("Physics2DDebugLayer") {
//   enable_alpha_blending_ = true;
// }

// void Physics2DDebugLayer::Render(const RenderContext& render_context) {
//   BeginDraw(render_context);

//   PhysicsWorld2D* physics_world_2d = render_context.scene->GetController<PhysicsWorld2D>("PhysicsWorld2D");
//   if (physics_world_2d != nullptr) {
//     physics_world_2d->world_.SetDebugDraw(this);
//     physics_world_2d->world_.DebugDraw();
//     physics_world_2d->world_.SetDebugDraw(nullptr);
//   }

//   EndDraw();
// }

// void Physics2DDebugLayer::DrawPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) {
//   std::vector<Vector3> positions(vertex_count);
//   for (int32 i = 0; i < vertex_count; ++i) {
//     positions[i] = ConvertVector(vertices[i]);
//   }
//   DrawLoop(positions.data(), positions.size(), ConvertColor(color));
// }

// void Physics2DDebugLayer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) {
//   std::vector<Vector3> positions(vertex_count);
//   for (int32 i = 0; i < vertex_count; ++i) {
//     positions[i] = ConvertVector(vertices[i]);
//   }
//   DrawConvexPolygon(positions.data(), vertex_count, GetSolidColor(color));
//   DrawLoop(positions.data(), vertex_count, ConvertColor(color));
// }

// void Physics2DDebugLayer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
//   PrimitiveRenderer::DrawCircle(ConvertVector(center), radius, ConvertColor(color));
// }

// void Physics2DDebugLayer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis,
//                                           const b2Color& color) {
//   const Color converted_color = ConvertColor(color);

//   Vector3 converted_center = ConvertVector(center);
//   PrimitiveRenderer::DrawDisc(converted_center, radius, GetSolidColor(color));
//   PrimitiveRenderer::DrawCircle(converted_center, radius, converted_color);
//   PrimitiveRenderer::DrawLine(converted_center, converted_center + radius * ConvertVector(axis), converted_color);
// }

// void Physics2DDebugLayer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
//   DrawLine(ConvertVector(p1), ConvertVector(p2), ConvertColor(color));
// }

// void Physics2DDebugLayer::DrawTransform(const b2Transform& xf) {
//   const Vector3 center = ConvertVector(xf.p);
//   DrawLine(center, center + transform_line_length_ * Vector3{xf.q.c, xf.q.s, 0.0f}, Color{1.0f, 0.0f, 0.0f, 1.0f});
//   DrawLine(center, center + transform_line_length_ * Vector3{-xf.q.s, xf.q.c, 0.0f}, Color{0.0f, 1.0f, 0.0f, 1.0f});
// }

// void Physics2DDebugLayer::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
//   PrimitiveRenderer::DrawPoint(ConvertVector(p), size, ConvertColor(color));
// }

}  // namespace ovis