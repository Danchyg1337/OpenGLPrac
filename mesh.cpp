#pragma once
Vertex::Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 text) {
	position = pos;
	normal = norm;
	texCoords = text;
}

Mesh::Mesh(std::vector<Vertex>::iterator verticesBegin, GLuint size, std::vector<Texture> textures) {
	verts.assign(verticesBegin, verticesBegin+size);
	texts = textures;
	Setup();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {
	verts = vertices;
	inds = indices;
	texts = textures;
	Setup();
}
void Mesh::Draw(Shader* shader) {
	GLuint diffuseN = 1, specularN = 1, reflectN = 1;
	glBindVertexArray(VAO);
	for (GLuint t = 0; t < texts.size(); t++) {
		glActiveTexture(GL_TEXTURE0 + t);

		const char* name = texts[t].type.data();
		char num[2] = { '0', '\0' };									//unsafe but fast;
	
		if (strcmp(name, "texture_diffuse")) {
			num[0] += diffuseN++;
		}
		else if (strcmp(name,"texture_specular")) {
			num[0] += specularN++;
		}
		else if (strcmp(name,"texture_reflect")) {
			num[0] += reflectN++;
		}
		char unif[50] = "material.";
		strcat_s(unif, name);
		strcat_s(unif, num);
		glUniform1i(glGetUniformLocation(shader->Program, unif), t);
		glBindTexture(GL_TEXTURE_2D, texts[t].id);
	}
	if(!inds.empty())
		glDrawElements(GL_TRIANGLES, inds.size(), GL_UNSIGNED_INT, 0);
	else glDrawArrays(GL_TRIANGLES, 0, verts.size());
	for (GLuint t = 0; t < texts.size(); t++) {
		glActiveTexture(GL_TEXTURE0 + t);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glBindVertexArray(0);
}

void Mesh::Setup() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);
	if (!inds.empty()) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(GLuint), &inds[0], GL_STATIC_DRAW);
	}
	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	//normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	

	//instancing
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}