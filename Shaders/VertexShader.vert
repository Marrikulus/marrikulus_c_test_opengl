#version 400

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
  texCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}

