#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColour;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_WorldPos;
out vec3 v_Normal;

void main()
{
    vec4 worldPos = u_Model * vec4(aPosition, 1.0);
    v_WorldPos    = worldPos.xyz;
    v_Normal      = mat3(transpose(inverse(u_Model))) * aNormal;
    gl_Position   = u_Projection * u_View * worldPos;
}

#shader fragment
#version 330 core

in  vec3 v_WorldPos;
in  vec3 v_Normal;
out vec4 FragColor;

uniform vec3  u_LightDir;
uniform float u_TileSize;

void main()
{
    float tileX = floor(v_WorldPos.x / u_TileSize);
    float tileZ = floor(v_WorldPos.z / u_TileSize);
    float check = mod(tileX + tileZ, 2.0);

    vec3 colorA    = vec3(0.80, 0.80, 0.80);  // light grey
    vec3 colorB    = vec3(0.55, 0.55, 0.55);  // medium grey
    vec3 baseColor = mix(colorA, colorB, check);

    float diffuse = max(dot(normalize(v_Normal), normalize(u_LightDir)), 0.0);
    float light   = 0.55 + 0.45 * diffuse;

    FragColor = vec4(baseColor * light, 1.0);
}
