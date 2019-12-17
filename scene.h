#pragma once
class Scene {
public:
	std::vector<Model*> models;
	std::vector<PointLight*> PointLights;
	std::optional<SunLight*> sun;
	std::optional <FlashLight*> flashlight;

	//std::mutex mut_modelsLoad;
	//std::vector<std::future<void>> v_futures;

	float gamma = 1.0, exposure = 2.2;

	glm::mat4 projection;
	glm::mat4 view;

	Cubemap* skybox;
	Shader* cubemapShader;
	Shader* screenShader;
	Shader* shaderNormals;
	Shader* shaderGBuffer;
	Shader* shaderGBufferShow;
	Shader* shaderGBufferDefaultShading;

	Model* sphere;

	glm::vec3 CameraPosition;
	glm::vec3 CameraDirection;

	bool showNormals = false;

	Scene(GLuint screenWidth, GLuint screenHeight);
	GLuint AddModel(Model* model);

	GLuint FindModel(Model* mdl);

	bool DrawToFramebuffer(GLuint frmBuffer);
	void DrawToScreen_Shader(Shader* shader);
	void DrawToScreen_Texture(GLuint* texture);
	void DrawToScreen();
	void DrawToGBuffer_Screen();

	void UpdateModels();

	bool DeleteModel(GLuint &id);
	//bool DeleteModel(const char* path);
	void ShowGBuffer(GLuint fbo);

	void LoadModels();
	void Clear();
	std::vector<Model*> simpleModels;
	std::vector<Model*> TrModels;
	void SetUniforms(Model *mdl, Shader* custom=nullptr);

private:
	GLuint fboHDR, rboHDR, tboHDR;
	GLuint fboG, rboG, tboGc, tboGn, tboGp;
	GLuint screenQuadVao;
	GLuint width, height;
	void LoadSingleModel(Model* mdl);
	void Draw(Shader* toAllModels = nullptr);
	void SortTransparent();
	void ShowNormals(Model *mdl);
	
	std::map<float, Model*> transparentModels;

	float ScreenVertices[24] = {
	//pos			//texPos
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f,  1.0f,  1.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f
	};
};