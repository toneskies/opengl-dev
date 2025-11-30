#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;


out vec4 colour;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS  = 3;

struct Light {
    vec3 colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight {
    Light base;
    vec3 direction;
};

struct PointLight {
    Light base;
    vec3 position;
    float constant;
    float linear;
    float exponent;
};

struct SpotLight {
    PointLight base;
    vec3 direction;
    float edge;        // cutoff in cos(angle)
};

struct Material {
    float specularIntensity;
    float shininess;   // use 16–32 for soft looks
};

uniform sampler2D theTexture;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform int pointLightCount;
uniform int spotLightCount;

uniform Material material;
uniform vec3 eyePosition;

uniform vec3 objectColor;

vec3 CalcLight(Light light, vec3 lightDir)
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightDir);

    // Ambient
    vec3 ambient = light.colour * light.ambientIntensity;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = light.colour * light.diffuseIntensity * diff;

    // Blinn–Phong Specular
    vec3 viewDir = normalize(eyePosition - FragPos);
    vec3 halfDir = normalize(L + viewDir);

    float spec = pow(max(dot(N, halfDir), 0.0), material.shininess);
    vec3 specular = light.colour * material.specularIntensity * spec;

    return ambient + diffuse + specular;
}

vec3 CalcDirectionalLight()
{
    return CalcLight(directionalLight.base, -directionalLight.direction);
}

vec3 CalcPointLight(PointLight p)
{
    vec3 lightDir = p.position - FragPos;

    float dist = length(lightDir);
    float attenuation = 1.0 / (p.constant + p.linear * dist + p.exponent * dist * dist);

    return CalcLight(p.base, lightDir) * attenuation;
}

vec3 CalcSpotLight(SpotLight s)
{
    vec3 lightDir = s.base.position - FragPos;
    vec3 L = normalize(lightDir);

    // Spotlight angle factor
    float theta = dot(L, normalize(-s.direction));
    if (theta < s.edge) 
        return vec3(0.0);

    // Smooth falloff: makes it soft and nice
    float eps = 0.1;   // edge softness
    float intensity = smoothstep(s.edge, s.edge + eps, theta);

    return CalcPointLight(s.base) * intensity;
}

void main()
{
    vec3 lighting = vec3(0);

    lighting += CalcDirectionalLight();

    for(int i = 0; i < pointLightCount; i++)
        lighting += CalcPointLight(pointLights[i]);

    for(int i = 0; i < spotLightCount; i++)
        lighting += CalcSpotLight(spotLights[i]);

    colour = texture(theTexture, TexCoord) * finalColour * vec4(objectColor, 1.0);
}
