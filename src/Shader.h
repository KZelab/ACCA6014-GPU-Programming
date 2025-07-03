#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};


class Shader
{
private:
	std::string m_Filepath;
	unsigned int m_RendererID;
	std::unordered_map< std::string, int> m_uniformLocationCache;

public:
	Shader(const std::string& filepath);
	Shader(const char* fragmentShaderSource, const char* vertexShaderSource);
	~Shader();

	void Bind() const;
	void Unbind() const;


	//less primitive shaders would be able to handle a lot more types of uniforms programatically determined

	
	void setUniform1i(const std::string& name, int i);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);


	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniform3f(const std::string& name, float v0, float v1, float v2);
	void setUniform2f(const std::string& name, float v0, float v1);
	void setUniform1f(const std::string& name, float v0);


private:
	unsigned int compileShader(unsigned int type, const std::string& source);
	ShaderProgramSource parseShaders(const std::string& filepath);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int getUniformLocation(const std::string& name);
};

