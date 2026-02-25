#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_LightSpaceMatrix * u_Model * vec4(aPosition, 1.0);
}

#shader fragment
#version 330 core

void main()
{
    // Depth is written automatically by OpenGL
}
