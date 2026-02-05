#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    // Transform vertex position to world space
    FragPos = vec3(u_Model * vec4(aPosition, 1.0));

    // Transform normal to world space using the normal matrix
    // This handles non-uniform scaling correctly
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;

    // Transform to clip space
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core

// Material parameters
uniform vec3 u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;

// Light parameters
uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform float u_LightIntensity;

// Camera position
uniform vec3 u_CameraPosition;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

const float PI = 3.14159265359;

// Normal Distribution Function (GGX/Trowbridge-Reitz)
// Describes the distribution of microfacet normals
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Geometry function (Schlick-GGX)
// Describes self-shadowing of microfacets
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

// Smith's method combines geometry obstruction and shadowing
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel equation (Schlick approximation)
// Describes the ratio of reflected vs refracted light
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(u_CameraPosition - FragPos);

    // Calculate reflectance at normal incidence (F0)
    // For dielectrics (non-metals), use 0.04 as a reasonable approximation
    // For metals, use the albedo color as F0
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, u_Albedo, u_Metallic);

    // Calculate per-light radiance
    vec3 L = normalize(u_LightPosition - FragPos);
    vec3 H = normalize(V + L);
    float distance = length(u_LightPosition - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = u_LightColor * u_LightIntensity * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, u_Roughness);
    float G = GeometrySmith(N, V, L, u_Roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // Calculate specular and diffuse contributions
    vec3 kS = F; // Specular reflection coefficient (Fresnel)
    vec3 kD = vec3(1.0) - kS; // Diffuse coefficient
    kD *= 1.0 - u_Metallic; // Metals have no diffuse reflection

    // Cook-Torrance specular term
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // Lambertian diffuse term
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * u_Albedo / PI + specular) * radiance * NdotL;

    // Ambient lighting (simplified)
    vec3 ambient = vec3(0.03) * u_Albedo * u_AO;

    vec3 color = ambient + Lo;

    // HDR tonemapping (Reinhard)
    color = color / (color + vec3(1.0));

    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
