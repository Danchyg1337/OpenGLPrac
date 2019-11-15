#pragma once
class Model {
public:
	glm::mat4 model;
	glm::mat4 MVP;
	glm::mat4 normalMatrix;
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(1, 1, 1);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	glm::vec3 lightColor;
	std::optional<std::string> path;
	Shader* shader;
	bool transparent;
	bool cullFace = false;
	GLenum cullFaceType;
	std::optional<Cubemap*> cubemap;

	Model();
	Model(const char* path, bool transpParts = false);
	Model(Mesh &msh, bool transpParts = false);
	void Draw();
	void DrawShader(Shader* shdr);

	void loadModel(std::string path);
	std::vector<Mesh> meshes;
	void EnableCullFace(GLenum cullFaceType = GL_CW);
	void DisableCullFace();
private:
	
	std::string directory;
	std::vector<std::string> textures_loaded;

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};

unsigned int a = 5;
double d = 4.56677;
float f;
char dfdsf = '4';
std::string r4445 = "dasdasd";
