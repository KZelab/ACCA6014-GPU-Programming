#shader vertex // Specifies the vertex shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

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

out vec3 VertexColor;

void main()
{
    //vertex position in world space
    vec3 FragPos = vec3(u_Model * vec4(aPosition, 1.0));

    //transform the normal to world space
    vec3 norm = normalize(mat3(transpose(inverse(u_Model))) * aNormal);

    //ambient component
    vec3 ambient = u_AmbientIntensity * u_Light.Colour;

    //Diffuse component
    vec3 lightDir = normalize(u_Light.Position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_DiffuseIntensity * diff * u_Light.Colour;

    //Specular component
    vec3 viewDir = normalize(u_CameraPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir),0.0), u_Shininess);
    vec3 specular = u_SpecularIntensity * spec * u_Light.Colour;

    //compute the final vertex colour using phong lighting model
    VertexColor = ambient + diffuse + specular;

    //Transform it into clip space 
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
};


#shader fragment // Specifies the fragment shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3

in vec3 VertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(VertexColor, 1.0);
}
