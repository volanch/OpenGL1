#version 330 core
in vec3 ePos, eNrm;
uniform vec3 kd;
uniform bool pickMode;
uniform vec3 pickColor;
out vec4 color;
void main() {
    if (pickMode) { color = vec4(pickColor, 1.0); return; }
    vec3 lightPos = vec3(0.0, 3.0, 2.0);
    vec3 N = normalize(eNrm), L = normalize(lightPos - ePos), V = normalize(-ePos);
    vec3 R = reflect(-L, N);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), 40.0);
    color = vec4(0.12 * kd + diff * kd + 0.3 * spec, 1.0);
}
