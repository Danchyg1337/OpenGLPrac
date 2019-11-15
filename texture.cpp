#pragma once
Texture::Texture(const char *texName, GLenum activeTexture, GLenum channelType, std::string ttype, bool flip) {
	stbi_set_flip_vertically_on_load(flip);
	texture = stbi_load(texName, &width, &height, &channels, STBI_rgb_alpha);
	glActiveTexture(activeTexture);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, channelType, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texture);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_set_flip_vertically_on_load(false);
	acT = activeTexture;
	type = ttype;
}

void Texture::bind() {
	glActiveTexture(acT);
	glBindTexture(GL_TEXTURE_2D, id);
}

bool Texture::setRepeatMode(GLenum glType, int XorY) {
	bind();
	if (XorY == 1 || XorY == 2) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S + XorY - 1, glType);
		return 1;
	}
	if (XorY == 3) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glType);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glType);
		return 1;
	}
	return 0;
}

bool Texture::setFilteringMode(GLenum glType, int MGorMN) {
	glActiveTexture(acT);
	if (MGorMN == 1 || MGorMN == 2) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER + (MGorMN - 1), glType);
		return 1;
	}
	if (MGorMN == 3) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glType);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glType);
		return 1;
	}
	return 0;
}

Cubemap::Cubemap(std::string texts[6]) {
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, NULL, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (int t = 0; t < 6; t++) {
		side = stbi_load(texts[t].c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (side) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + t, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, side);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << texts[t] << std::endl;
		}
		stbi_image_free(side);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::Bind() {
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void Cubemap::Unbind() {
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
