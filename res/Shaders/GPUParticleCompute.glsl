#version 430 core

layout(local_size_x = 256) in;

struct Particle
{
    vec2 pos;
    vec2 vel;
    vec4 color;
    float life;
    float maxLife;
    float size;
    float _pad; // padding to align to 16 bytes
};

layout(std430, binding = 0) buffer ParticleBuffer
{
    Particle particles[];
};

uniform float u_DeltaTime;
uniform float u_Gravity;
uniform vec2  u_EmitterPos;
uniform int   u_MaxParticles;
uniform float u_Time;
uniform float u_EmissionRate;
uniform vec4  u_ColorStart;
uniform vec4  u_ColorEnd;
uniform float u_SpeedMin;
uniform float u_SpeedMax;
uniform float u_LifeMin;
uniform float u_LifeMax;
uniform float u_SizeMin;
uniform float u_SizeMax;

// Hash-based PRNG
uint hash(uint x)
{
    x ^= x >> 16u;
    x *= 0x45d9f3bu;
    x ^= x >> 16u;
    x *= 0x45d9f3bu;
    x ^= x >> 16u;
    return x;
}

float randFloat(uint seed)
{
    return float(hash(seed)) / float(0xFFFFFFFFu);
}

float randRange(float lo, float hi, uint seed)
{
    return lo + randFloat(seed) * (hi - lo);
}

void main()
{
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= uint(u_MaxParticles))
        return;

    Particle p = particles[idx];

    if (p.life > 0.0)
    {
        // Update living particle
        p.life -= u_DeltaTime;

        if (p.life > 0.0)
        {
            p.vel.y += u_Gravity * u_DeltaTime;
            p.pos += p.vel * u_DeltaTime;
        }
    }
    else
    {
        // Dead particle: probabilistic respawn based on emission rate
        // Expected spawns this frame = emissionRate * dt
        // Probability per dead particle ≈ (emissionRate * dt) / maxParticles
        float spawnChance = (u_EmissionRate * u_DeltaTime) / float(u_MaxParticles);

        // Generate a random value for this particle
        uint seed = hash(idx * 1973u + uint(u_Time * 6547.0));
        float roll = randFloat(seed);

        if (roll < spawnChance)
        {
            // Respawn particle
            uint s0 = hash(seed + 1u);
            uint s1 = hash(seed + 2u);
            uint s2 = hash(seed + 3u);
            uint s3 = hash(seed + 4u);
            uint s4 = hash(seed + 5u);
            uint s5 = hash(seed + 6u);

            p.pos = u_EmitterPos;
            p.life = randRange(u_LifeMin, u_LifeMax, s0);
            p.maxLife = p.life;
            p.size = randRange(u_SizeMin, u_SizeMax, s1);

            float angle = randRange(0.0, 6.28318530718, s2);
            float speed = randRange(u_SpeedMin, u_SpeedMax, s3);
            p.vel = vec2(cos(angle), sin(angle)) * speed;

            float t = randFloat(s4);
            p.color = mix(u_ColorStart, u_ColorEnd, t);
        }
    }

    particles[idx] = p;
}
