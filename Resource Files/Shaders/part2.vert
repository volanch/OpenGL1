#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nrm;
uniform mat4 model, view, proj;
out vec3 ePos, eNrm;
void main() {
    vec4 p = view * model * vec4(pos, 1.0);
    ePos = p.xyz;
    eNrm = mat3(view * model) * nrm;
    gl_Position = proj * p;
}
