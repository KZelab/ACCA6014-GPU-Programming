#shader vertex // Specifies the vertex shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3

layout(location = 0) in vec3 aPosition; // Vertex position
layout(location = 1) in vec3 aNormal;   // Vertex normal

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;    // Fragment position in world space
out vec3 Normal;     // Normal in world space

void main() {
    // Transform vertex position to world space:
    // Given the model matrix u_Model, we transform aPosition (local space)
    // to world space coordinates. This ensures that the fragment shader
    // has the correct world position for lighting calculations.
    FragPos = vec3(u_Model * vec4(aPosition, 1.0));

    // Correctly transform normal to world space:
    // Since the normal is a direction and not a position, we need to apply
    // the transpose of the inverse of the model matrix.
    // This ensures the normal remains accurate if non-uniform scaling is applied.
    // Mathematical foundation: 
    // Normal' = transpose(inverse(M)) * Normal
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;

    // Transform to clip space:
    // The final vertex position is projected onto screen space using
    // the Model-View-Projection (MVP) transformation.
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}


#shader fragment
#version 330 core

#define MAX_LIGHTS 16

// Light definition
struct Light {
    vec3 position;
    vec3 direction;
    vec3 colour;
    float intensity;
    int type;    // 0 = point, 1 = directional, 2 = spotlight
    float cutoff;
};

// Uniforms sent from CPU
uniform Light uLights[MAX_LIGHTS];
uniform int uLightCount;

uniform vec3 uCameraPosition;
uniform float uAmbientIntensity;
uniform float uDiffuseIntensity;
uniform float uSpecularIntensity;
uniform float uShininess;

// Inputs from vertex shader
in vec3 FragPos;
in vec3 Normal;

// Output final colour
out vec4 FragColor;

void main() {
    vec3 result = vec3(0.0);

    for (int i = 0; i < uLightCount; i++) {
        vec3 norm = normalize(Normal);
        vec3 lightDir;

        // Determine light direction depending on type
        if (uLights[i].type == 1) {
            // Directional light uses negative direction vector
            lightDir = normalize(-uLights[i].direction);
        } else {
            // Point and spot lights calculate direction from light to fragment
            lightDir = normalize(uLights[i].position - FragPos);
        } 

        // Ambient light (constant contribution)
        vec3 ambient = uAmbientIntensity * uLights[i].colour;

        // Diffuse shading (Lambert)
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = uDiffuseIntensity * diff * uLights[i].colour;

        // Specular shading (Phong)
        vec3 viewDir = normalize(uCameraPosition - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
        vec3 specular = uSpecularIntensity * spec * uLights[i].colour;

        // Spotlight effect (cutoff angle check)
        if (uLights[i].type == 2) {
            float theta = dot(lightDir, normalize(-uLights[i].direction));
            if (theta > uLights[i].cutoff) {
                float falloff = (theta - uLights[i].cutoff) / (1.0 - uLights[i].cutoff);
                diffuse *= falloff;
                specular *= falloff;
            } else {
                diffuse = vec3(0.0);
                specular = vec3(0.0);
            }
        }

        // Combine the contributions, scaled by intensity
        result += (ambient + diffuse + specular) * uLights[i].intensity;
    }

    FragColor = vec4(result, 1.0); // Output final colour
}
