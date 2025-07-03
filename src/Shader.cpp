#include "Shader.h"

#include "Renderer.h"
Shader::Shader(const std::string& filepath) : m_Filepath(filepath), m_RendererID(0)
{
    //std::string fp = R"(C:\Users\natha\Desktop\code\CPP\CMakeHelloWorld\res\shaders\Basic.shader)";
    ShaderProgramSource source = parseShaders(filepath);
    // Create and compile the shader program from the vertex and fragment shaders
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}
Shader::Shader(const char* fragmentShaderSource, const char* vertexShaderSource)
{
    m_RendererID = CreateShader(vertexShaderSource, fragmentShaderSource);
}
Shader::~Shader()
{
    GlCall(glDeleteProgram(m_RendererID));// Delete the shader program after we're done using it
}
void Shader::Bind() const
{
    GlCall(glUseProgram(m_RendererID));
}
void Shader::Unbind() const
{
    GlCall(glUseProgram(0));
}


void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GlCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2)
{
    GlCall(glUniform3f(getUniformLocation(name), v0, v1, v2));
}

void Shader::setUniform2f(const std::string& name, float v0, float v1)
{
    GlCall(glUniform2f(getUniformLocation(name), v0, v1));
}

void Shader::setUniform1f(const std::string& name, float v0)
{
    GlCall(glUniform1f(getUniformLocation(name), v0));
}



void Shader::setUniform1i(const std::string& name, int i)
{
    GlCall(glUniform1i(getUniformLocation(name), i));
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GlCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}
unsigned int Shader::getUniformLocation(const std::string& name)
{
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
    {
        return m_uniformLocationCache[name];
    }
    GlCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "WARNING:: Uniform " << name << " is not defined \n";
    
    m_uniformLocationCache[name] = location;

    return location;
}
ShaderProgramSource Shader::parseShaders(const std::string& filepath)
{
    if (!std::filesystem::exists(filepath)) {
        std::cout << "File does not exist." << std::endl;
    }

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];//stack allocated array 

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }
    std::cout << ss[0].str() << ss[1].str() << std::endl;
    return { ss[0].str(), ss[1].str() };
}
// Function to compile a shader of a given type (vertex or fragment)
unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
    // Create a shader object and get its ID
    unsigned int id = glCreateShader(type);
    std::cout << "Shader id:" << id << std::endl;

    // Convert the shader source code from std::string to C-string
    const char* src = source.c_str();

    // Associate the source code with the shader object
    glShaderSource(id, 1, &src, nullptr);

    // Compile the shader
    glCompileShader(id);

    // Check if the compilation was successful
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        // If compilation failed, retrieve the error message length
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        // Allocate space for the error message
        char* message = (char*)_malloca(length * sizeof(char));

        // Get the error message
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader: " <<
            (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader" << std::endl;
        std::cout << message << std::endl;

        // Delete the shader object as it failed to compile
        glDeleteShader(id);
        return 0;
    }

    return id;
}
// Function to create a shader program by linking a vertex and fragment shader
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // Create a shader program and get its ID
    unsigned int program = glCreateProgram();
    // Compile the vertex shader
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    // Compile the fragment shader
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    // Attach the compiled shaders to the program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    // Link the shaders together into a complete program
    glLinkProgram(program);
    // Validate the linked program to ensure it's usable
    glValidateProgram(program);
    return program;
}

