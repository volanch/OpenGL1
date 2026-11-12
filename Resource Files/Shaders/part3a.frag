#version 330 core
in vec3 vPosEye;
in vec3 vNormalEye;
in vec2 vUV;
out vec4 FragColor;

struct Light {
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float enabled;
};

struct Material {
    vec4 specular;
    float shininess;
};

uniform Light uLight[2];
uniform Material uMaterial;
uniform sampler2D uTexture;

vec4 phongShade(vec3 P, vec3 N, vec4 kd)
{
    vec3 V = normalize(-P);
    if (dot(N, V) < 0.0) N = -N;
    vec4 color = vec4(0.0);

    for (int i = 0; i < 2; ++i) {
        if (uLight[i].enabled < 0.5) continue;

        vec3  L = normalize(uLight[i].position - P);
        float NdotL = max(dot(N, L), 0.0);

        vec4 ambient = uLight[i].ambient * kd;
        vec4 diffuse = uLight[i].diffuse * kd * NdotL;
        vec4 specular = vec4(0.0);
        if (NdotL > 0.0) {
            vec3 R = reflect(-L, N);
            specular = uLight[i].specular * uMaterial.specular
                     * pow(max(dot(R, V), 0.0), uMaterial.shininess);
        }
        color += ambient + diffuse + specular;
    }
    color.a = 1.0;
    return clamp(color, 0.0, 1.0);
}

void main()
{
    vec4 kd = vec4(texture(uTexture, vUV).rgb, 1.0);
    FragColor = phongShade(vPosEye, normalize(vNormalEye), kd);
}
