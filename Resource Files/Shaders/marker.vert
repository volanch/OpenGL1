#version 330 core
uniform mat4 uModelView;
uniform mat4 uProjection;
uniform vec3 uPos;

void main()
{
    gl_Position = uProjection * uModelView * vec4(uPos, 1.0);
    gl_PointSize = 10.0;
}
