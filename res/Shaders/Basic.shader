#shader vertex
#version 330 core

// =============================================================================
// VERTEX ATTRIBUTES
// =============================================================================
/**
 * INPUT ATTRIBUTE TYPES:
 * Declare attributes as the ACTUAL data type being sent from the CPU.
 * OpenGL/GLSL will handle the conversion to vec4 for gl_Position.
 *
 * For 2D positions: use vec2
 * For 3D positions: use vec3
 *
 * GLSL VEC4 CONSTRUCTORS:
 * GLSL provides flexible constructors to build larger vectors from smaller ones:
 *
 *   vec4(vec2, float, float)  ->  vec4(xy, z, w)
 *   vec4(vec3, float)         ->  vec4(xyz, w)
 *   vec4(float, vec3)         ->  vec4(x, yzw)
 *   vec4(vec2, vec2)          ->  vec4(xy, zw)
 *
 * THE W COMPONENT:
 * For positions, W should be 1.0 (points in homogeneous coordinates).
 * For directions/normals, W should be 0.0 (vectors, no translation).
 *
 * HOMOGENEOUS COORDINATES:
 * The 4x4 MVP matrix uses homogeneous coordinates where:
 *   - W = 1.0: Point in space (affected by translation)
 *   - W = 0.0: Direction vector (NOT affected by translation)
 *
 * After MVP multiplication, the GPU performs PERSPECTIVE DIVIDE:
 *   final_position = gl_Position.xyz / gl_Position.w
 *
 * This is how perspective projection creates the "things get smaller
 * with distance" effect - farther objects have larger W after projection.
 */

// Declare as vec2 since we're sending 2D position data from CPU
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoords;

out vec2 v_TexCoords;

uniform mat4 u_MVP;

void main()
{
    // Construct vec4 from vec2: (x, y, z=0, w=1)
    // Z = 0.0 places the vertex on the XY plane
    // W = 1.0 marks this as a point (not a direction)
    gl_Position = u_MVP * vec4(position, 0.0, 1.0);

    // For 3D positions, you would use:
    // layout(location = 0) in vec3 position;
    // gl_Position = u_MVP * vec4(position, 1.0);

    v_TexCoords = texCoords;
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 colour;

in vec2 v_TexCoords;

uniform sampler2D u_Texture;

void main()
{
    vec4 texColour = texture(u_Texture, v_TexCoords);
    colour = texColour;
}
