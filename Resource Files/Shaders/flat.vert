#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aFaceNormal;
layout (location = 2) in vec3 aSmoothNormal;

uniform mat4 uModelView;
uniform mat4 uProjection;

out vec3 vColor;

void main()
{
    vColor = abs(aFaceNormal);
    gl_Position = uProjection * uModelView * vec4(aPos, 1.0);
}
