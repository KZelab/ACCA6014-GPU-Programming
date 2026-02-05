#shader vertex
#version 330 core
// Vertex attributes matching our Vertex struct layout
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;    // Not used, but part of layout
layout(location = 2) in vec3 aColour;    // Not used, but part of layout
layout(location = 3) in vec2 aTexCoord;

void main() {
    gl_Position = vec4(aPosition, 1.0);
}




#shader fragment

#version 330

out vec4 FragColor;

uniform float uTime;

uniform vec2 uResolution;



vec3 palette(float t)
{
    vec3 a = vec3(0.5,0.5,0.5);
    vec3 b = vec3(0.5,0.5,0.5);
    vec3 c = vec3(1.0,1.0,1.0);
    vec3 d = vec3(0.263,0.416,0.557);

    return a + b * cos(6.28318 * (c * t * d));
}

void main()
{
    vec2 uv = (gl_FragCoord.xy - 0.5 * uResolution.xy) / uResolution.y;
    vec2 uv0 = uv;
    vec3 finalColour = vec3(0.0);

    for(float i = 0.0; i < 4.0; i++)
    {
        uv = fract(uv * 1.5) - 0.5;

        float d = length(uv) * exp(-length(uv0));
        vec3 colour2 = palette(length(uv0) + i*.4 + uTime*.4);
        d = sin(d *8. + uTime) /8.;
        d = abs(d);
        
        d = pow(0.01 / d, 1.2);

        finalColour += colour2 * d ;
    }

    //colour2 *= d;

    FragColor = vec4(finalColour, 1.0); // Colour output based on texture coordinates
}

