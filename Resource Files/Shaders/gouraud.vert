#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aFaceNormal;
layout (location = 2) in vec3 aSmoothNormal;

uniform mat4 uModelView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;
uniform float uNormalSign;

out vec4 vColor;

struct Light {
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float enabled;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform Light uLight[2];
uniform Material uMaterial;

vec4 phongShade(vec3 P, vec3 N)
{
    vec3 V = normalize(-P);
    vec4 color = vec4(0.0);

    for (int i = 0; i < 2; ++i) {
        if (uLight[i].enabled < 0.5) continue;

        vec3  L = normalize(uLight[i].position - P);
        float NdotL = max(dot(N, L), 0.0);

        vec4 ambient = uLight[i].ambient * uMaterial.ambient;
        vec4 diffuse = uLight[i].diffuse * uMaterial.diffuse * NdotL;
        vec4 specular = vec4(0.0);
        if (NdotL > 0.0) {
            vec3 R = reflect(-L, N);
            specular = uLight[i].specular * uMaterial.specular
                     * pow(max(dot(R, V), 0.0), uMaterial.shininess);
        }
        color += ambient + diffuse + specular;
    }
    color.a = uMaterial.diffuse.a;
    return clamp(color, 0.0, 1.0);
}

void main()
{
    vec4 P = uModelView * vec4(aPos, 1.0);
    vec3 N = normalize(uNormalMatrix * (uNormalSign * aSmoothNormal));

    vColor = phongShade(P.xyz, N);
    gl_Position = uProjection * P;
}
