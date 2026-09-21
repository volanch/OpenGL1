#version 330 core

in vec3 fColor;
out vec4 FragColor;

uniform float uNightMix;
uniform vec3 uNightTint;

void main()
{
	vec3 nightColor = fColor * uNightTint;
	vec3 finalColor = mix(fColor, nightColor, uNightMix);
	FragColor = vec4(finalColor, 1.0f);
}
