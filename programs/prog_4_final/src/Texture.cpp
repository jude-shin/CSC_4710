#include "Texture.h"
#include "GLSL.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

Texture::Texture() :
	filename(""),
	tid(0)
{
	
}

Texture::~Texture()
{
	
}

void Texture::init()
{
	int w, h, sourceComponents;

	stbi_set_flip_vertically_on_load(true);

	unsigned char *data = stbi_load(
			filename.c_str(),
			&w,
			&h,
			&sourceComponents,
			STBI_rgb_alpha   // Always give OpenGL 4 channels
			);

	if (!data) {
		cerr << filename << " not found" << endl;
		return;
	}

	width = w;
	height = h;

	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);

	CHECKED_GL_CALL(glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data
			));

	CHECKED_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

void Texture::setWrapModes(GLint wrapS, GLint wrapT)
{
	// Must be called after init()
	CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, tid));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT));
}

void Texture::bind(GLint handle)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, tid);
	glUniform1i(handle, unit);
}

void Texture::unbind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}
