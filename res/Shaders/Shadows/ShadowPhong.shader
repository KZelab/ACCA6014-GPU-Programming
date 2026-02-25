#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_LightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

void main()
{
    FragPos = vec3(u_Model * vec4(aPosition, 1.0));
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    FragPosLightSpace = u_LightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core

struct Light
{
    vec3 Direction;
    vec3 Colour;
};

uniform Light u_Light;
uniform vec3 u_CameraPosition;

uniform float u_AmbientIntensity;
uniform float u_DiffuseIntensity;
uniform float u_SpecularIntensity;
uniform float u_Shininess;

uniform vec3 u_ObjectColor;

uniform sampler2D u_ShadowMap;
uniform float u_ShadowBias;
uniform int u_EnablePCF;
uniform int u_PCFKernelSize;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

out vec4 FragColor;

float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Remap from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // If outside the light frustum, no shadow
    if (projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    // Slope-scale bias to reduce shadow acne
    float bias = max(u_ShadowBias * (1.0 - dot(normal, lightDir)), u_ShadowBias * 0.1);

    float shadow = 0.0;

    if (u_EnablePCF == 1)
    {
        // PCF: sample NxN texels around the fragment
        vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
        int halfKernel = u_PCFKernelSize / 2;
        int samples = 0;
        for (int x = -halfKernel; x <= halfKernel; ++x)
        {
            for (int y = -halfKernel; y <= halfKernel; ++y)
            {
                float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                samples++;
            }
        }
        shadow /= float(samples);
    }
    else
    {
        // Hard shadows: single sample
        float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    return shadow;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-u_Light.Direction);

    // Ambient
    vec3 ambient = u_AmbientIntensity * u_Light.Colour;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_DiffuseIntensity * diff * u_Light.Colour;

    // Specular (Phong)
    vec3 viewDir = normalize(u_CameraPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    vec3 specular = u_SpecularIntensity * spec * u_Light.Colour;

    // Shadow
    float shadow = CalculateShadow(FragPosLightSpace, norm, lightDir);

    // Ambient always applied; shadow only affects diffuse + specular
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * u_ObjectColor;

    FragColor = vec4(lighting, 1.0);
}
