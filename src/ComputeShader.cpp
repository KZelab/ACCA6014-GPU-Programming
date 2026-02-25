#include "ComputeShader.h"
#include "Renderer.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

/**
 * COMPUTE SHADERS — What are they?
 * ---------------------------------
 * A compute shader is a GPU program that runs OUTSIDE the normal graphics pipeline.
 * Unlike vertex/fragment shaders which are tied to rendering geometry, a compute
 * shader is general-purpose: you give it data, it processes it in massively parallel
 * fashion, and writes results back to a buffer.
 *
 * WHY INHERIT FROM Shader?
 * -------------------------
 * A compute shader program is still an OpenGL program object (glCreateProgram).
 * It still uses glUseProgram to bind, glGetUniformLocation to find uniforms, and
 * glUniform* to set them. All of that is inherited from Shader unchanged.
 *
 * The only two differences are:
 *   1. Construction — one GL_COMPUTE_SHADER stage instead of vertex+fragment
 *   2. Execution — glDispatchCompute() instead of draw calls
 *
 * We use the protected default constructor Shader() which initializes m_RendererID
 * to 0 without triggering the vertex+fragment parsing. Then we do our own
 * single-stage compilation here.
 *
 * PIPELINE COMPARISON:
 *   Traditional:  CPU -> Vertex Shader -> Fragment Shader -> Framebuffer
 *   Compute:      CPU -> Compute Shader -> SSBO (read back or use in next draw)
 *
 * REQUIRES: OpenGL 4.3+ (that's why we bumped from 3.3 to 4.3)
 */

ComputeShader::ComputeShader(const std::string& filepath)
	// Calls the protected Shader() default constructor — sets m_RendererID = 0
	// without compiling any vertex/fragment shaders
	: Shader()
{
	m_Filepath = filepath;
	std::string source = ReadFile(filepath);
	unsigned int shader = Compile(source);

	// A compute shader still needs a "program" object, just like vertex/fragment shaders.
	// The program is what gets bound with glUseProgram() and holds uniform state.
	// The difference is we only attach ONE shader instead of two.
	m_RendererID = glCreateProgram();
	glAttachShader(m_RendererID, shader);
	glLinkProgram(m_RendererID);
	glValidateProgram(m_RendererID);

	int success;
	glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char log[512];
		glGetProgramInfoLog(m_RendererID, 512, nullptr, log);
		std::cerr << "Compute shader program link failed: " << log << std::endl;
	}

	// The shader object is baked into the program after linking — we don't need it anymore.
	// This is the same as what Shader::CreateShader does, but it never called glDeleteShader
	// (a minor leak in the original code). We clean up properly here.
	glDeleteShader(shader);
}

// --- Everything below is inherited from Shader: ---
// Bind()              — glUseProgram(m_RendererID)
// Unbind()            — glUseProgram(0)
// setUniform1i/1f/2f/3f/4f/Mat4f — glUniform* with cached locations
// ~Shader()           — glDeleteProgram(m_RendererID)
// No code needed — that's the point of inheritance.

/**
 * glDispatchCompute — The compute shader equivalent of a draw call.
 *
 * Instead of "draw N triangles", this says "launch X * Y * Z work groups".
 * Each work group runs a fixed number of threads (defined in the shader with
 * layout(local_size_x = 256) — so 256 threads per group in our case).
 *
 * Total threads = groupsX * groupsY * groupsZ * local_size_x * local_size_y * local_size_z
 *
 * For our particle system with 100,000 particles and local_size_x=256:
 *   numGroups = ceil(100000 / 256) = 391
 *   Total threads = 391 * 256 = 100,096 (the extra 96 threads check bounds and exit early)
 *
 * Each thread knows its global index via gl_GlobalInvocationID.x in the shader,
 * so thread 0 updates particle 0, thread 1 updates particle 1, etc.
 * All 100k particles are updated simultaneously on the GPU — this is why it's fast.
 *
 * IMPORTANT: After dispatching, the results aren't immediately visible to other
 * shader stages. You need glMemoryBarrier() before reading the SSBO in a draw call.
 */
void ComputeShader::Dispatch(unsigned int groupsX, unsigned int groupsY, unsigned int groupsZ) const
{
	GlCall(glDispatchCompute(groupsX, groupsY, groupsZ));
}

// Simple file read — unlike Shader::parseShaders, we don't need to split
// the file into vertex/fragment sections. A compute shader is just one
// continuous block of GLSL code, so we read the whole file as-is.
std::string ComputeShader::ReadFile(const std::string& filepath)
{
	if (!std::filesystem::exists(filepath))
		std::cerr << "Compute shader file does not exist: " << filepath << std::endl;

	std::ifstream stream(filepath);
	std::stringstream ss;
	ss << stream.rdbuf();
	return ss.str();
}

/**
 * Compile a compute shader from source.
 *
 * This mirrors Shader::compileShader but uses GL_COMPUTE_SHADER instead of
 * GL_VERTEX_SHADER / GL_FRAGMENT_SHADER. The compilation pipeline is identical:
 *   1. glCreateShader(type)    — create a shader object
 *   2. glShaderSource(...)     — attach source code
 *   3. glCompileShader(...)    — compile GLSL to GPU machine code
 *   4. glGetShaderiv(...)      — check for errors
 *
 * The compiled shader object is then attached to a program and linked (in the
 * constructor above). After linking, the shader object can be deleted.
 */
unsigned int ComputeShader::Compile(const std::string& source)
{
	unsigned int id = glCreateShader(GL_COMPUTE_SHADER);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cerr << "Failed to compile compute shader:\n" << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
