#pragma once
#include <GL/glew.h>

class Framebuffer {
public:
	Framebuffer(int width, int height, bool depthOnly = false);
	~Framebuffer();
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	void Bind() const;
	void Unbind() const;
	unsigned int GetDepthTexture() const { return m_DepthTexture; }
	unsigned int GetColorTexture() const { return m_ColorTexture; }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	bool CheckStatus() const;

private:
	unsigned int m_RendererID = 0;
	unsigned int m_DepthTexture = 0;
	unsigned int m_ColorTexture = 0;
	int m_Width, m_Height;
	bool m_DepthOnly;
};
