// g++ -Wall main.cpp -L/usr/local/lib -lglfw -lglfw3 -lrt -lm -lXrandr -lXrender -lXi -lGL -lpthread -pthread -lm -ldl -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp -ldrm -lGLEW -lGLU -lassimp -lstdc++ -lz



//Scene class it is like more renderer than actual scene.

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <optional>
#include <cmath>
#include <chrono>
//#include <future>
//#include <mutex>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#ifdef __linux__
#include "imgui/imgui.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_widgets.cpp"
#include "imgui/imgui_impl_glfw.cpp"
#include "imgui/imgui_impl_opengl3.cpp"
#include "imgui/imgui_demo.cpp"
#elif _WIN32
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#endif


#include "shader.h"
#include "light.h"
#include "camera.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "scene.h"
#include "vfx.h"
#include "texture.cpp"
#include "mesh.cpp"
#include "model.cpp"
#include "scene.cpp"
#include "primitives.h"
#include "vfx.cpp"

#include "MyImGuiFuncs.h"

#define WIDTH 1600
#define HEIGHT 900
#define MOUSE_SENS 0.05f
#define CAM_SPEED 5.f

//#define DEBUG

#ifdef DEBUG 
static int allocs = 0;
void* operator new(size_t size) {
	allocs++;
	//std::cout << "Allocated " << size << " bytes\n";
	return malloc(size);
}
#endif

bool keys[1024];
bool flashlight = false;
bool firstmouse = true;
bool cursoreActive = false;
int BLUR = 5, blurWeightNum = 5;
float pitch, yaw, lastX, lastY;
float blurA = 2;

glm::mat4 model;
glm::vec3 lightPos(0.0f, 11.0f, 1.0f);  //light
glm::vec3 cameraPos = glm::vec3(3, 12, 0);
glm::vec3 normCursore;
Scene* scene;

void GetFPS(float& fps);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int modes);
void DoMovement(float fElapsedTime);
void MouseEvent(GLFWwindow* window, double MouseX, double MouseY);

void PrintErrors(){
	int errorcode = glGetError();
	while(errorcode!=0){
		std::cout<<errorcode<<std::endl;
		errorcode = glGetError();
	}
	std::cout<<"End of PrintError()\nPress any key\n";
	std::cin.get();
}

int main() {
	GLFWwindow *gwin;
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);

	gwin = glfwCreateWindow(WIDTH, HEIGHT, "Hello", NULL, NULL);
	glfwMakeContextCurrent(gwin);

	glfwSetKeyCallback(gwin, KeyCallback);
	glfwSetCursorPosCallback(gwin, MouseEvent);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (glewInit() != GLEW_OK) {
		std::cout << ":C\n";
	}
	
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glfwSwapInterval(0);

	scene = new Scene(WIDTH, HEIGHT);
	Camera camera(cameraPos, glm::radians(90.0f), (float)WIDTH / (float)HEIGHT, 0.01f, 1000.0f);


	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	glm::vec3 glassPositions[] = {
		glm::vec3(-3.1f, 7.0f, 1.9f),
		glm::vec3(-3.0f, 7.0f, 1.0f),
		glm::vec3(-3.0f, 7.0f, 0.0f)
	};

	std::vector<PointLight*> lightPositions = {
		new PointLight(glm::vec3(-3.0f, 7.0f, -2.0f), glm::vec3(2.0f, 1.0f, 1.0f)),
		new PointLight(glm::vec3(5.0f, 3.0f, -7.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		new PointLight(glm::vec3(-7.5f, 12.2f, -.5f), glm::vec3(1.0f, 0.0f, 1.0f)),
		new PointLight(glm::vec3(-6.8f, 4.0f, -7.3f), glm::vec3(1.0f, 1.0f, 0.0f)),
		new PointLight(glm::vec3(7.4f, 4.4f, -3.5f), glm::vec3(0.0f, 1.0f, 1.0f))
	};


	for (int t = 0; t < lightPositions.size(); t++) {
		lightPositions[t]->SetAttenuation(1.0f, 0.045, 0.0075);
	}
	SunLight* sun = new SunLight(glm::vec3(-.4f, -1.0f, -.3f), glm::vec3(1.0f, 1.0f, 1.0f));
	FlashLight* fLight = new FlashLight(normCursore, cameraPos, glm::vec3(1.0f));
	fLight->SetAttenuation(1.0f, 0.045, 0.0075);
	scene->flashlight = fLight;

	GLuint fboM, rbo, tboM;
	glGenFramebuffers(1, &fboM);
	glBindFramebuffer(GL_FRAMEBUFFER, fboM);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glGenTextures(1, &tboM);
	glBindTexture(GL_TEXTURE_2D, tboM);
	glTextureStorage2D(tboM, 10, GL_RGB16F, WIDTH, HEIGHT);
	glGenerateTextureMipmap(tboM);
	glTextureParameteri(tboM, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(tboM, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(tboM, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(tboM, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tboM, 0);

	Shader* shader1 = new Shader("shaders/mysh.glsl");
	Shader* shader3 = new Shader("shaders/mysh3.glsl");
	Shader* shaderSQ = new Shader("shaders/square.glsl");
	Shader* shaderCubemap = new Shader("shaders/cubemap.glsl");
	Shader* shaderRefl = new Shader("shaders/reflection.glsl");
	Shader* shaderRefract = new Shader("shaders/refraction.glsl");
	Shader* shaderBlur = new Shader("shaders/screen.glsl");
	Shader* shader2Buffers = new Shader("shaders/screen2.glsl");
	Shader* shader2Buffers2 = new Shader("shaders/screen3.glsl");
	Shader* shaderNormals = new Shader("shaders/showNormals.glsl");
	Shader* shaderREMIND = new Shader("shaders/trytoremind.glsl");

	Texture tex1 = Texture("textures/container.png", GL_TEXTURE0, GL_RGB, "texture_diffuse");
	Texture tex2 = Texture("textures/container_specular.png", GL_TEXTURE0, GL_RGB, "texture_specular");
	Texture tex3 = Texture("textures/grass.png", GL_TEXTURE0, GL_RGBA, "texture_diffuse", true);
	tex3.setRepeatMode(GL_CLAMP_TO_BORDER, 3);
	Texture tex4 = Texture("textures/blue.png", GL_TEXTURE0, GL_RGBA, "texture_diffuse");

	std::string cubemtex[] = { "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg","skybox/front.jpg" ,"skybox/back.jpg" };
	Cubemap* texCubeMap = new Cubemap(cubemtex);

	//cubes
	Mesh cubeMesh(prmtvs::Cube.verts.begin(), prmtvs::Cube.verts.size(), std::vector<Texture>({tex1, tex2}));
	Mesh lightCubeMesh(prmtvs::Cube.verts.begin(), prmtvs::Cube.verts.size(), std::vector<Texture>({}));
	Mesh floor(prmtvs::Cube.verts.begin()+30, 6, std::vector<Texture>({ tex1, tex2 }));
	Mesh grass(prmtvs::Cube.verts.begin(), 6, std::vector<Texture>({ tex3 }));
	Mesh glass(prmtvs::Cube.verts.begin(), 6, std::vector<Texture>({ tex4 }));

	Model* grassM = new Model(grass);
	grassM->shader = shader1;
	srand(glfwGetTime());
	grassM->position = glm::vec3(0, 1, 0);
	GLuint grassID = scene->AddModel(grassM);

	Model* spider = new Model();
	spider->path = "models/OBJ/spider.obj";
	spider->position = glm::vec3(0, 4, 0);
	spider->shader = shader1;
	spider->scale = glm::vec3(.03, .03, .03);
	spider->EnableCullFace(GL_CW);
	//GLuint spiderID = scene->AddModel(spider);


	Model* cat = new Model();
	cat->path = "models/cat/Cat_Statue.fbx";
	cat->position = glm::vec3(0, -3.5, 0);
	cat->shader = shader1;
	cat->rotation = glm::vec3(-90.f, 0., 0.);
	cat->EnableCullFace(GL_CW);
	GLuint catID = scene->AddModel(cat);


	Model* nano = new Model();
	nano->path = "models/nanosuit/nanosuit.obj";
	nano->position = glm::vec3(0, 10, 0);
	nano->shader = shader1;
	nano->scale = glm::vec3(.3, .3, .3);
	nano->EnableCullFace(GL_CW);
	nano->cubemap = texCubeMap;
	GLuint nanoID = scene->AddModel(nano);

	Model* mod = new Model();
	mod->path = "models/mod/model.obj";
	mod->position = glm::vec3(5, 13, 0);
	mod->shader = shader1;
	mod->scale = glm::vec3(3, 3, 3);
	mod->EnableCullFace(GL_CW);
	mod->cubemap = texCubeMap;
	//GLuint modID = scene->AddModel(mod);


	Model* floorM = new Model(floor);
	floorM->position = glm::vec3(0.0f, -4.0f, 0.0f);
	floorM->scale = glm::vec3(50.0f, 1.0f, 50.0f);
	floorM->shader = shader1;
	GLuint floorID = scene->AddModel(floorM);

	unsigned cubeCnt = sizeof(cubePositions) / sizeof(glm::vec3);
	std::vector<GLuint> cubeID;
	for (GLuint t = 0; t < cubeCnt; t++) {
		Model* cubeM = new Model(cubeMesh);
		model = glm::translate(glm::mat4(1.0f), cubePositions[t]);
		cubeM->position = cubePositions[t];
		cubeM->rotation = glm::vec3((float)glfwGetTime() * 5.0f * cubePositions[t]);
		cubeM->shader = shader1;
		cubeM->EnableCullFace();
		cubeID.push_back(scene->AddModel(cubeM));
	}

	std::vector<GLuint> lightCubeID;
	for (unsigned int a = 0; a < lightPositions.size(); a++) {
		Model* lightCubeM = new Model(lightCubeMesh);
		lightCubeM->position = lightPositions[a]->position;
		lightCubeM->lightColor = lightPositions[a]->color;
		lightCubeM->shader = shader3;
		lightCubeM->EnableCullFace();
		lightPositions[a]->modelID = lightCubeM;
		lightCubeID.push_back(scene->AddModel(lightCubeM));
	}

	unsigned int glassCnt = sizeof(glassPositions) / sizeof(glm::vec3);
	std::vector<GLuint> glassID;
	for (unsigned int t = 0; t < glassCnt; t++) {
		Model* glassM = new Model(glass, true);
		glassM->shader = shader1;
		glassM->position = glassPositions[t];
		glassID.push_back(scene->AddModel(glassM));
	}

	scene->LoadModels();						//important call, models wont load without it.
	scene->skybox = texCubeMap;
	scene->cubemapShader = shaderCubemap;
	scene->PointLights = lightPositions;
	//scene->sun = sun;


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui_ImplGlfw_InitForOpenGL(gwin, true);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool show_demo_window = false;
	double fElapsedTime = 0;
	float X = 2, A = 2;
	float funcW = 1/(A*sqrt(2*M_PI))*std::exp((-X*X)/(2*A*A));

	/*{std::lock_guard<std::mutex> lock(scene->mut_modelsLoad);
	std::cout << "Simple models: \n";
	for (int t = 0; t < scene->simpleModels.size(); t++) {
		if (scene->simpleModels.at(t)->path.has_value())
			std::cout << *scene->simpleModels.at(t)->path << std::endl;
		else std::cout << "Without path" << std::endl;
	}
	std::cout << "Transparent models: \n";
	for (int t = 0; t < scene->TrModels.size(); t++) {
		if (scene->TrModels.at(t)->path.has_value())
			std::cout << *scene->TrModels.at(t)->path << std::endl;
		else std::cout << "Without path" << std::endl;
	}
	system("pause");
	}*/
	float FPS;
	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();
	while (!glfwWindowShouldClose(gwin)) {
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		fElapsedTime = elapsedTime.count();
		
		//dynamicCubeMap->GenerateCubemap(glm::vec3(0, 4, 0), scene);
		GetFPS(FPS);
		DoMovement(fElapsedTime);
		scene->UpdateModels();
		if (cursoreActive) glfwSetInputMode(gwin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else glfwSetInputMode(gwin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		camera.update(cameraPos, normCursore);

		scene->projection = camera.GetProjection();
		scene->view = camera.GetView();
		scene->CameraPosition = cameraPos;
		scene->CameraDirection = normCursore;
		(*scene->flashlight)->active = flashlight;
		///////////////////////////////////////////////////////////////////// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ShowOverlayFPS(nullptr, &FPS);
		ShowOverlayMatrice(nullptr, glm::value_ptr(scene->view), "View");
		if (cursoreActive) {
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			ShowWindowTree(&scene->models, &lightPositions, &scene->gamma, &scene->exposure, &BLUR, &blurWeightNum, &blurA);
		}

		/////////////////////////////////////////////////////////////////////

		//cubes drawing
		{//std::lock_guard<std::mutex> lock(scene->mut_modelsLoad);
		for (GLuint t = 0; t < cubeCnt; t++) {
			float time = (float)glfwGetTime()*t+1;
			scene->models.at(cubeID.at(t))->position = glm::vec3(scene->models.at(cubeID.at(t))->position.x + sin(time)*0.03, scene->models.at(cubeID.at(t))->position.y, scene->models.at(cubeID.at(t))->position.z + cos(time)*0.03);
			scene->models.at(cubeID.at(t))->rotation = glm::vec3(time * 5.0f * scene->models.at(cubeID.at(t))->position);
		}
		for (GLuint t = 0; t < lightCubeID.size(); t++) {
			scene->models.at(lightCubeID.at(t))->position = lightPositions.at(t)->position;
			scene->models.at(lightCubeID.at(t))->lightColor = lightPositions.at(t)->color;
		}
		}

		float levels[]{5.0};

		//scene->showNormals = true;
		//scene->DrawToFramebuffer(fboM);
		//scene->DrawToScreen();

		//VFX::GetInstance()->BlurBrightAreas(tboM, fboM, 1., blurWeightNum, blurA, BLUR, levels, 1);
		//scene->DrawToScreen_Texture(&tboM);
		scene->DrawToGBuffer_Screen();
		//scene->ShowGBuffer(0);

		#ifdef DEBUG
			std::cout << allocs << " Allocations\n";
			allocs = 0;
			system("pause");
		#endif
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(gwin);
		glfwPollEvents();
	}
	shader1->DeleteShader();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void GetFPS(float& fps) {
	static float previous_seconds = glfwGetTime();
	static int frame_count;
	float current_seconds = glfwGetTime();
	float elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25) {
		previous_seconds = current_seconds;
		fps = (float)frame_count / elapsed_seconds;
		frame_count = 0;
	}
	frame_count++;
}

void DoMovement(float fElapsedTime) {
	if (keys[GLFW_KEY_W]) {
		cameraPos += normCursore * CAM_SPEED * fElapsedTime;
	}
	if (keys[GLFW_KEY_A]) {
		cameraPos += glm::normalize(glm::cross(glm::vec3(0, 1, 0), normCursore)) * CAM_SPEED * fElapsedTime;
	}
	if (keys[GLFW_KEY_S]) {
		cameraPos -= normCursore * CAM_SPEED * fElapsedTime;
	}
	if (keys[GLFW_KEY_D]) {
		cameraPos -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), normCursore)) * CAM_SPEED * fElapsedTime;
	}
	if (keys[GLFW_KEY_F]) {
		flashlight = !flashlight;
	}
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int modes) {
	if (action == GLFW_PRESS) {
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = false;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		if (cursoreActive)
			cursoreActive = false;
		else cursoreActive = true;
	}
}

void MouseEvent(GLFWwindow* window, double MouseX, double MouseY) {
	if (cursoreActive) return;
	glm::vec3 cursore;
	if (firstmouse) {
		lastX = MouseX;
		lastY = MouseY;
		firstmouse = false;
	}

	float offsetX = (MouseX - lastX) * MOUSE_SENS;
	float offsetY = (lastY - MouseY) * MOUSE_SENS;
	lastX = MouseX;
	lastY = MouseY;

	yaw += offsetX;
	pitch += offsetY;

	if (pitch > 89.0f) pitch = 89.0;
	if (pitch < -89.0f) pitch = -89.0;

	cursore.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	cursore.y = glm::sin(glm::radians(pitch));
	cursore.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	normCursore = glm::normalize(cursore);
}