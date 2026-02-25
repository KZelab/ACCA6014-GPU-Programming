#pragma once

#include "Shader.h"

/**
 * ComputeShader — inherits from Shader
 *
 * Since compute shaders use the same OpenGL program object, the same
 * glUseProgram for binding, and the same uniform system, there's no reason
 * to duplicate all of that. The only differences are:
 *
 *   1. CONSTRUCTION: A compute shader compiles a single GL_COMPUTE_SHADER
 *      stage from one .glsl file, rather than parsing "#shader vertex" /
 *      "#shader fragment" sections and compiling two stages.
 *
 *   2. EXECUTION: Instead of being used with draw calls (glDrawArrays etc.),
 *      a compute shader is launched with glDispatchCompute().
 *
 * Everything else — Bind(), Unbind(), setUniform*(), the destructor, the
 * uniform location cache — is inherited directly from Shader with zero changes.
 *
 * The protected default constructor Shader() lets us skip the vertex+fragment
 * compilation path and handle our own single-stage compilation here.
 */
class ComputeShader : public Shader
{
public:
	ComputeShader(const std::string& filepath);

	void Dispatch(unsigned int groupsX, unsigned int groupsY, unsigned int groupsZ) const;

private:
	std::string ReadFile(const std::string& filepath);
	unsigned int Compile(const std::string& source);
};
