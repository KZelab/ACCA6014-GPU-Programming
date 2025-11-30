#include "Texture.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

Texture::Texture(const std::string& filepath) :
	m_RendererID(0), m_Filepath(filepath), m_Localbuffer(nullptr), width(0), height(0), bitsPerPixel(0)
{

	// Disable vertical flip for texture atlas (may need to adjust UV coordinates if this was intended)
	stbi_set_flip_vertically_on_load(0);

	m_Localbuffer = stbi_load(filepath.c_str(), &width, &height, &bitsPerPixel, 4);

	if (m_Localbuffer == nullptr)
	{
		std::cerr << "!!! FAILED TO LOAD TEXTURE: " << filepath << std::endl;
		std::cerr << "stb_image error: " << stbi_failure_reason() << std::endl;
		// You could throw an exception here
		return;
	}

	// Debug: Check if texture has any non-black pixels
	bool hasColor = false;
	for (int i = 0; i < width * height * 4; i++)
	{
		if (m_Localbuffer[i] > 10) // Check if any channel has significant value
		{
			hasColor = true;
			break;
		}
	}
	if (!hasColor)
	{
		std::cerr << "WARNING: Texture appears to be entirely black!" << std::endl;
	}
	else
	{
		std::cout << "DEBUG: Texture has color data (sample pixel 0: R="
			<< (int)m_Localbuffer[0] << ", G=" << (int)m_Localbuffer[1]
			<< ", B=" << (int)m_Localbuffer[2] << ", A=" << (int)m_Localbuffer[3] << ")" << std::endl;
	}


	GlCall(glGenTextures(1, &m_RendererID));
	GlCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	// --- Add a check for GL_RGBA format ---
	GLint internalFormat = GL_RGBA8;
	GLenum dataFormat = GL_RGBA;

	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GlCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));


	GlCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, m_Localbuffer));

	// Good practice to reset it to default after, though not strictly necessary here
	GlCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));

	GlCall(glBindTexture(GL_TEXTURE_2D, 0));

	GlCall(glFinish());


	if (m_Localbuffer)
	{
		stbi_image_free(m_Localbuffer);
	}

	std::cout << "Texture class: Loaded texture: " << filepath << " ("
		<< width << "x" << height << ", " << bitsPerPixel << " bpp)" << std::endl;
}

Texture::~Texture()
{
	GlCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GlCall(glActiveTexture(GL_TEXTURE0 + slot));
	GlCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

}

void Texture::Unbind() const
{
	GlCall(glBindTexture(GL_TEXTURE_2D, 0));
}