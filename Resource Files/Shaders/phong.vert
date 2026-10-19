#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aFaceNormal;
layout (location = 2) in vec3 aSmoothNormal;

uniform mat4 uModelView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;
uniform float uNormalSign;

out vec3 vPosEye;
out vec3 vNormalEye;

void main()
{
    vec4 P = uModelView * vec4(aPos, 1.0);
    vPosEye = P.xyz;
    vNormalEye = uNormalMatrix * (uNormalSign * aSmoothNormal);
    gl_Position = uProjection * P;
}
