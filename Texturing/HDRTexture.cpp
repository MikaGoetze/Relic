#include "HDRTexture.h"
#include <Texturing/stb_image.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "Core/Util.h"

HDRTexture::HDRTexture(std::string path)
{
	stbi_set_flip_vertically_on_load(true);
	int num_channels;
	float * image_data = stbi_loadf(path.c_str(), &width, &height, &num_channels, 0);

	if(image_data)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, image_data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(image_data);
	}
	else
	{
		Util::Log("[Relic][HDRImage] Error : failed to load HDR image '" + path + "'.");
	}
}

HDRTexture::~HDRTexture()
{
}
