#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in float a_AO;

out vec2 v_TexCoord;
out vec3 v_Normal;
out float v_AO;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main()
{
    v_TexCoord = a_TexCoord;
    v_Normal = a_Normal;
    v_AO = a_AO;
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#shader fragment
#version 330 core

in vec2 v_TexCoord;
in vec3 v_Normal;
in float v_AO;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec3 u_LightDir;

void main()
{
    // DEBUG: Uncomment these one at a time to test different things

    //Test 1: Pink screen means geometry is rendering
     //FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    // return;

    // Test 2: UV coordinates as colors (red = U, green = V)
    // Amplify V by 16x to see if it's just too small
    // FragColor = vec4(v_TexCoord.x, v_TexCoord.y * 16.0, 0.0, 1.0);
    // return;

    // Test 3: Ambient occlusion visualization (should be white/gray)
    // FragColor = vec4(v_AO, v_AO, v_AO, 1.0);
    // return;

     //Test 4: Normals as colors (map -1..1 to 0..1 for visibility)
     //FragColor = vec4(v_Normal.x * 0.5 + 0.5, v_Normal.y * 0.5 + 0.5, v_Normal.z * 0.5 + 0.5, 1.0);
     //return;

    // Test 5: Raw texture color without lighting
    // vec4 texColor = texture(u_Texture, v_TexCoord);
    // FragColor = vec4(texColor.rgb, 1.0);
    // return;

    // Test 6: Sample a specific UV location (center of first texture)
    // vec4 texColor = texture(u_Texture, vec2(0.03125, 0.03125)); // Center of index 0
    // FragColor = vec4(texColor.rgb, 1.0);
    // return;

    // ACTUAL RENDERING CODE:
    vec4 texColor = texture(u_Texture, v_TexCoord);

    // Apply ambient occlusion
    vec3 aoColor = texColor.rgb * v_AO;

    // Simple directional lighting
    float light = max(dot(v_Normal, normalize(u_LightDir)), 0.3);
    vec3 finalColor = aoColor * light;

    FragColor = vec4(finalColor, texColor.a);
}