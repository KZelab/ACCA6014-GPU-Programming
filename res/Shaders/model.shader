#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

out vec3 v_Normal;
out vec3 v_FragPos;
out vec2 v_TexCoord;
out vec3 v_ModelPos;  // Position in model space (before transformation)

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    // Transform vertex position
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;

    // Pass model-space position (for debug visualization)
    v_ModelPos = a_Position;

    // Transform normal to world space
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;

    // Pass texture coordinates to fragment shader
    v_TexCoord = a_TexCoord;

    gl_Position = u_ViewProjection * worldPos;
}

#shader fragment
#version 330 core

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;
in vec3 v_ModelPos;

out vec4 FragColor;

uniform vec3 u_Color;        // Model color (fallback if no texture)
uniform vec3 u_LightDir;     // Light direction
uniform vec3 u_ViewPos;      // Camera position
uniform sampler2D u_Texture; // Model texture
uniform int u_UseTexture;    // 1 = use texture, 0 = use color

void main()
{
    // Base color from texture or uniform
    vec3 baseColor;
    if (u_UseTexture == 1)
    {
        baseColor = texture(u_Texture, v_TexCoord).rgb;
    }
    else
    {
        baseColor = u_Color;
    }

    // Normalize the normal vector
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightDir);

    // Ambient lighting
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * baseColor;

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * baseColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * vec3(1.0);

    // Combine lighting components
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
