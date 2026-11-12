#version 330 core
in vec3 vPosWorld;
in vec3 vPosEye;
in vec3 vNormalEye;
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
uniform vec3 uCenter;
uniform float uRadius;

float hash(vec3 p) { return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453); }

float noise(vec3 p)
{
    vec3 i = floor(p), f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float x00 = mix(hash(i), hash(i + vec3(1, 0, 0)), f.x);
    float x10 = mix(hash(i + vec3(0, 1, 0)), hash(i + vec3(1, 1, 0)), f.x);
    float x01 = mix(hash(i + vec3(0, 0, 1)), hash(i + vec3(1, 0, 1)), f.x);
    float x11 = mix(hash(i + vec3(0, 1, 1)), hash(i + vec3(1, 1, 1)), f.x);
    return mix(mix(x00, x10, f.y), mix(x01, x11, f.y), f.z);
}

float fbm(vec3 p)
{
    float sum = 0.0, amp = 0.5;
    for (int k = 0; k < 5; k++) { sum += amp * noise(p); p *= 2.03; amp *= 0.5; }
    return sum;
}

vec3 marble(vec3 p)
{
    float warp = fbm(p * 2.5);
    float vein = abs(sin(4.0 * p.x + 3.0 * p.y + 7.0 * warp));
    vec3 stone = mix(vec3(0.10, 0.20, 0.42), vec3(0.92, 0.88, 0.80),
                     smoothstep(0.25, 0.75, fbm(p * 4.0 + 3.0)));
    vec3 gold = vec3(0.95, 0.65, 0.15);
    return mix(gold, stone, smoothstep(0.0, 0.22, vein));
}

vec4 phongShade(vec3 P, vec3 N, vec4 kd)
{
    vec3 V = normalize(-P);
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
    vec3 p = (vPosWorld - uCenter) / uRadius * 1.25;    // world coordinates, normalised to the model size
    vec4 kd = vec4(marble(p), 1.0);
    FragColor = phongShade(vPosEye, normalize(vNormalEye), kd);
}
