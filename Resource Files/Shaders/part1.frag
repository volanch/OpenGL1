#version 330 core
in vec3 vcol;
out vec4 color;
void main() { color = vec4(vcol, 1.0); }
