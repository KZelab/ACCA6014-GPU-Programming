#shader vertex
#version 430 core

struct Particle
{
    vec2 pos;
    vec2 vel;
    vec4 color;
    float life;
    float maxLife;
    float size;
    float _pad;
};

layout(std430, binding = 0) buffer ParticleBuffer
{
    Particle particles[];
};

uniform mat4 u_MVP;

out vec4 v_Color;
out float v_Life;
out float v_MaxLife;

void main()
{
    Particle p = particles[gl_VertexID];

    // Dead particles get placed offscreen
    if (p.life <= 0.0)
    {
        gl_Position = vec4(-9999.0, -9999.0, 0.0, 1.0);
        gl_PointSize = 0.0;
        v_Color = vec4(0.0);
        v_Life = 0.0;
        v_MaxLife = 1.0;
        return;
    }

    gl_Position = u_MVP * vec4(p.pos, 0.0, 1.0);
    gl_PointSize = p.size;
    v_Color = p.color;
    v_Life = p.life;
    v_MaxLife = p.maxLife;
}


#shader fragment
#version 430 core

layout(location = 0) out vec4 fragColor;

in vec4 v_Color;
in float v_Life;
in float v_MaxLife;

void main()
{
    // Circular point: discard fragments outside radius
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float dist = dot(coord, coord);
    if (dist > 1.0)
        discard;

    // Soft edge
    float alpha = 1.0 - dist;

    // Fade based on remaining life
    float lifeRatio = v_Life / v_MaxLife;

    fragColor = vec4(v_Color.rgb, v_Color.a * alpha * lifeRatio);
}
