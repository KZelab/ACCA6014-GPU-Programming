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
protected:
	// Protected so subclasses (e.g. ComputeShader) can access the program ID
	// and uniform cache directly. These were originally private, but inheritance
	// requires the derived class to set m_RendererID during its own compilation.
	std::string m_Filepath;
	unsigned int m_RendererID;
	std::unordered_map< std::string, int> m_uniformLocationCache;

	// Protected default constructor — lets subclasses handle their own compilation
	// without triggering the vertex+fragment parsing path.
	// The public constructors below are for the normal vertex+fragment workflow.
	Shader() : m_RendererID(0) {}

public:
	Shader(const std::string& filepath);
	Shader(const char* fragmentShaderSource, const char* vertexShaderSource);
	virtual ~Shader();

	void Bind() const;
	void Unbind() const;


	//less primitive shaders would be able to handle a lot more types of uniforms programatically determined


	void setUniform1i(const std::string& name, int i);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);


	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniform3f(const std::string& name, float v0, float v1, float v2);
	void setUniform2f(const std::string& name, float v0, float v1);
	void setUniform1f(const std::string& name, float v0);

	unsigned int GetID() const { return m_RendererID; }

private:
	unsigned int compileShader(unsigned int type, const std::string& source);
	ShaderProgramSource parseShaders(const std::string& filepath);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int getUniformLocation(const std::string& name);
};

