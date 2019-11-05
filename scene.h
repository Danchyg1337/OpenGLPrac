#pragma once
class Scene {
public:
	std::vector<Model*> models;
	std::vector<PointLight*> PointLights;
	std::optional<SunLight*> sun;
	
	std::mutex mut_modelsLoad;
	std::vector<std::future<void>> v_futures;

	float gamma = 1.0, exposure = 2.2;

	glm::mat4 projection;
	glm::mat4 view;

	Cubemap* skybox;
	Shader* cubemapShader;
	Shader* screenShader;
	Shader* shaderNormals;

	glm::vec3 CameraPosition;

	bool showNormals = false;

	Scene(GLuint screenWidth, GLuint screenHeight);
	GLuint AddModel(Model* model);

	GLuint FindModel(Model* mdl);

	bool DrawToFramebuffer(GLuint frmBuffer);
	void DrawToScreen_Shader(Shader* shader);
	void DrawToScreen_Texture(GLuint* texture);
	void DrawToScreen();

	bool DeleteModel(GLuint &id);
	//bool DeleteModel(const char* path);


	void LoadModels();
	void Clear();
	std::vector<Model*> simpleModels;
	std::vector<Model*> TrModels;
	void SetUniforms(Model *mdl);
private:
	GLuint fboHDR = 0, rboHDR, tboHDR;
	GLuint screenQuadVao;
	GLuint width, height;
	void LoadSingleModel(Model* mdl);
	void Draw();
	void SortTransparent();
	void ShowNormals(Model *mdl);
	
	std::map<float, Model*> transparentModels;

	float ScreenVertices[24] = {
	//pos			//texPos
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f
	};
};