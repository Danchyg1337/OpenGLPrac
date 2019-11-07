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

void Cubemap::SetCubemap(std::string texts[6]) {
	vao_id = SetVAO();
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

void Cubemap::GenerateCubemap(glm::vec3 position, Scene* scene) {
	vao_id = SetVAO();
	glActiveTexture(GL_TEXTURE0);
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, cubemapSize, cubemapSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	for (int t = 0; t < 6; t++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + t, 0, GL_RGB, cubemapSize, cubemapSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_CUBE_MAP_POSITIVE_X, id, 0);
	for (int t = 0; t < 6; t++) {
		ScreenToTexture(t, position, fbo, scene);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + t, id, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Cubemap::ScreenToTexture(unsigned int side, glm::vec3 cameraPos, GLuint fbo, Scene* scene) {					
	Camera cmr = Camera(cameraPos, glm::radians(90.0f), 1, 0.01f, 50.0f);
	float pitch, yaw;
	switch (side) {
	case 0:
		pitch = 0;
		yaw = -90;
		break;
	case 1:
		pitch = 0;
		yaw = 90;
		break;
	case 2:
		pitch = 90;
		yaw = 0;
		break;
	case 3:
		pitch = -90;
		yaw = 0;
		break;
	case 4:
		pitch = 0;
		yaw = 0;
		break;
	case 5:
		pitch = 0;
		yaw = 180;
		break;
	}
	glm::vec3 cursore;
	cursore.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	cursore.y = glm::sin(glm::radians(pitch));
	cursore.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	cmr.update(cameraPos, glm::normalize(cursore));
	scene->projection = cmr.GetProjection();
	scene->view = cmr.GetView();
	scene->DrawToFramebuffer(fbo);
}

void Cubemap::Bind() {
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void Cubemap::Unbind() {
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLuint Cubemap::SetVAO() {
	VAO vao_cbmap(1);
	vao_cbmap.Bind();
	Buffer vbo_cubemap(1, GL_ARRAY_BUFFER);
	vbo_cubemap.Data(skyboxVertices, sizeof(skyboxVertices), GL_STATIC_DRAW);
	vbo_cubemap.VAP(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
	return vao_cbmap.ID;
}
