uniform mat4 u_WorldViewProjection;

in vec2 a_Position;
in vec2 a_TextureCoordinates;

out vec2 vs_TextureCoordinates;

void main() {
  gl_Position = vec4(a_Position, 0.0, 1.0) * u_WorldViewProjection;
  vs_TextureCoordinates = a_TextureCoordinates;
}