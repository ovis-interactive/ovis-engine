uniform mat4 u_ViewProjection;

in vec3 a_Position;
in vec4 a_Color;

out vec4 vs_Color;

void main() {
  gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
  vs_Color = a_Color;
}