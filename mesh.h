#pragma once
struct Vertex {
	Vertex() = default;
	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 text);
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh {
public:
	Mesh(std::vector<Vertex>::iterator verticesBegin, GLuint size, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	void Draw(Shader* shader);
	std::vector<Vertex> verts;
	std::vector<GLuint> inds;
	std::vector<Texture> texts;
private:
	void Setup();
	GLuint VAO, VBO, EBO;
};