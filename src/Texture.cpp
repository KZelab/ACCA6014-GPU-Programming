#include "Texture.h"
#include "vendor/stb_image.h"

Texture::Texture(const std::string& filepath):
	m_RendererID(0), m_Filepath(filepath), m_Localbuffer(nullptr),width(0), height(0), bitsPerPixel(0)
{
	
	stbi_set_flip_vertically_on_load(1);

	m_Localbuffer = stbi_load(filepath.c_str(), &width, &height, &bitsPerPixel, 4);

	GlCall(glGenTextures(1, &m_RendererID));
	GlCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE ));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE));

	GlCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Localbuffer));

	GlCall(glBindTexture(GL_TEXTURE_2D, 0));


	if (m_Localbuffer)
	{
		stbi_image_free(m_Localbuffer);
	}
}

Texture::~Texture()
{
	GlCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GlCall(glActiveTexture(GL_TEXTURE0+slot));
	GlCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

}

void Texture::Unbind() const
{
	GlCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}
