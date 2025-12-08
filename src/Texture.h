#pragma once


#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_Filepath;
	unsigned char* m_Localbuffer;
	int width, height, bitsPerPixel;
public:
	Texture(const std::string& filepath);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	int getWidth() const { return this->width; };
	int getHeight() const { return this->height; };

	glm::vec2 getTexelSize() const { return glm::vec2(1.0f / width, 1.0f / height); }
};