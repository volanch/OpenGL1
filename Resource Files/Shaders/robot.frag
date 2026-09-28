#version 330
in vec3 fColor;
out vec4 FragColor;

uniform float uTime;
uniform int uIsClose;
uniform vec3 uColorOverride;
uniform float uOverrideMix;

void main() {
vec3 col = fColor;

    if (uOverrideMix > 0.0){
        col = mix(col, uColorOverride, uOverrideMix);
    }

    if (uIsClose == 1){
        float pulse = 0.25 * (0.5 + 0.5 * sin(uTime * 8.0));
        col += vec3(pulse, pulse * 0.9, 0.0);
    }

    FragColor = vec4(col, 1.0f);
}