#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColour;
layout(location = 3) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec3 v_Colour;
out vec2 v_TexCoords;

void main()
{
    vec4 worldPos = u_Model * vec4(aPosition, 1.0);
    v_FragPos   = vec3(worldPos);

    // Transpose-inverse corrects normals under non-uniform scaling
    v_Normal    = mat3(transpose(inverse(u_Model))) * aNormal;

    v_Colour    = aColour;
    v_TexCoords = aTexCoords;

    gl_Position = u_Projection * u_View * worldPos;
}


#shader fragment
#version 330 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec3 v_Colour;
in vec2 v_TexCoords;

// Textures bound by ModelMesh::Draw
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

// Set to 1 when the mesh has the corresponding texture, 0 to fall back to vertex colour / white
uniform int u_UseDiffuseTexture;
uniform int u_UseSpecularTexture;

// Lighting
uniform vec3  u_LightPos;
uniform vec3  u_LightColor;
uniform vec3  u_CameraPos;

uniform float u_AmbientStrength;
uniform float u_SpecularStrength;
uniform float u_Shininess;

out vec4 FragColor;

void main()
{
    // -----------------------------------------------------------------------
    // Base colour
    // v_Colour defaults to (1,1,1) for Assimp-loaded meshes with no vertex
    // colours, so multiplying by the texture sample passes it through cleanly.
    // -----------------------------------------------------------------------
    vec3 baseColor = (u_UseDiffuseTexture != 0)
        ? texture(texture_diffuse1, v_TexCoords).rgb * v_Colour
        : v_Colour;

    vec3 norm     = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    vec3 viewDir  = normalize(u_CameraPos - v_FragPos);
    vec3 halfDir  = normalize(lightDir + viewDir);   // Blinn half-vector

    // Ambient
    vec3 ambient = u_AmbientStrength * u_LightColor;

    // Diffuse (Lambertian)
    float diff   = max(dot(norm, lightDir), 0.0);
    vec3  diffuse = diff * u_LightColor;

    // Specular (Blinn-Phong)
    vec3  specBase = (u_UseSpecularTexture != 0)
        ? texture(texture_specular1, v_TexCoords).rgb
        : vec3(1.0);
    float spec    = pow(max(dot(norm, halfDir), 0.0), u_Shininess);
    vec3  specular = u_SpecularStrength * spec * specBase * u_LightColor;

    vec3 result = (ambient + diffuse) * baseColor + specular;
    FragColor   = vec4(result, 1.0);
}
