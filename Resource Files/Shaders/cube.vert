#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 uTransform;
out vec3 vColor;

void main()
{
    gl_Position = uTransform * vec4(aPos, 1.0);
    gl_Position.z = -gl_Position.z;
    vColor = aColor;
}