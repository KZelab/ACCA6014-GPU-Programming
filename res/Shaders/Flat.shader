#shader vertex // Specifies the vertex shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;
flat out vec3 FaceNormal;

void main()
{
    // Transform vertex position to world space:
    // Given the model matrix u_Model, we transform aPosition (local space)
    // to world space coordinates. This ensures that the fragment shader
    // has the correct world position for lighting calculations.
    FragPos = vec3(u_Model * vec4(aPosition, 1.0)); //so this is the fragment positon in world space


    // Correctly transform normal to world space:
    // Since the normal is a direction and not a position, we need to apply
    // the transpose of the inverse of the model matrix.
    // This ensures the normal remains accurate if non-uniform scaling is applied.
    // Mathematical foundation: 
    // Normal' = transpose(inverse(M)) * Normal
    FaceNormal = mat3(transpose(inverse(u_Model))) * aNormal;

    // Transform to clip space:
    // The final vertex position is projected onto screen space using
    // the Model-View-Projection (MVP) transformation.
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
};


#shader fragment // Specifies the fragment shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3


struct Light
{
    vec3 Position; //this position is in world space
    vec3 Colour;
};

uniform Light u_Light;
uniform vec3 u_CameraPosition; //camera in world space


//Phong Lighting paramaters
uniform float u_AmbientIntensity;
uniform float u_DiffuseIntensity;
uniform float u_SpecularIntensity;
uniform float u_Shininess;

in vec3 FragPos;
flat in vec3 FaceNormal;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(FaceNormal);
    vec3 ambient = u_AmbientIntensity * u_Light.Colour;
    
    vec3 lightDir = normalize(u_Light.Position - FragPos); //direction from the fragment to the light
    float diff = max(dot(norm, lightDir),0.0); //Lambertian Reflectance aka Cosine of angle
    vec3 diffuse = u_DiffuseIntensity * diff * u_Light.Colour;    


    vec3 viewDir = normalize(u_CameraPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    vec3 specular = u_SpecularIntensity * spec * u_Light.Colour;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}
