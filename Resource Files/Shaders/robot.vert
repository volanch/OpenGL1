#version 330
layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec3 vColor;

out vec3 fColor;

uniform vec2 uOffset;
uniform float uScale;

void main() {
    vec2 pos = vPosition * uScale + uOffset;
    gl_Position = vec4(pos, 0.0, 1.0);
    fColor = vColor;
}