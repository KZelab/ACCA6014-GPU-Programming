#shader vertex // Specifies the vertex shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3

// Input attributes: 
// 'position' is a vec4 that defines the vertex position (layout location 0)
// 'texCoords' is a vec2 that defines the texture coordinates (layout location 1)
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoords;

// Output to the fragment shader: the interpolated texture coordinates
out vec2 v_TexCoords;


uniform mat4 u_MVP;//model view projection matrix 

void main()
{
   // Assign the input vertex position to the built-in output variable for position
   gl_Position = u_MVP * position;

   // Pass the texture coordinates to the fragment shader
   v_TexCoords = texCoords;
};


#shader fragment // Specifies the fragment shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3

// Output color of the fragment (layout location 0)
layout(location = 0) out vec4 colour;

// Input from the vertex shader: interpolated texture coordinates
in vec2 v_TexCoords;

// Uniforms (global shader variables):
// 'u_Texture' is a uniform sampler to sample the texture image

uniform sampler2D u_Texture;

void main()
{
    // Sample the texture color at the given texture coordinates
    vec4 texColour = texture(u_Texture, v_TexCoords);

    // Set the output color to the texture color
    colour = texColour;
};
