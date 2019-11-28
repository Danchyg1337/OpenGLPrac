#pragma once

class Model;
class Light {
public:
	glm::vec3 color;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	float constant = 0;
	float linear = 0;
	float quadratic = 0;

	Light() {
		ambient = glm::vec3(0.05f);
		diffuse = glm::vec3(0.5f);
		specular = glm::vec3(1.0f);
	}
};


class PointLight : public Light {
public:
	glm::vec3 position;
	std::optional<Model*> modelID;
	PointLight(glm::vec3 pos, glm::vec3 col) : position(pos){
		specular = color =  col;
		diffuse = col * 0.5f;
		ambient = col * 0.025f;
	}
	void SetAttenuation(float cnst, float lin, float qdc) {
		constant = cnst;
		linear = lin;
		quadratic = qdc;
	}

	void SetActive(Shader* shader, GLuint num) {
		shader->Use();
		specular = color;
		diffuse = color * 0.5f;
		ambient = color * 0.025f;
		std::string line = "Plight[" + std::to_string(num) + "].";
		glUniform3f(glGetUniformLocation(shader->Program, (line + "ambient").c_str()), ambient.x, ambient.y, ambient.z);
		glUniform3f(glGetUniformLocation(shader->Program, (line + "diffuse").c_str()), diffuse.x, diffuse.y, diffuse.z);
		glUniform3f(glGetUniformLocation(shader->Program, (line + "specular").c_str()), specular.x, specular.y, specular.z);
											  
		glUniform3f(glGetUniformLocation(shader->Program, (line + "position").c_str()), position.x, position.y, position.z);
											  
		glUniform1f(glGetUniformLocation(shader->Program, (line + "constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(shader->Program, (line + "linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(shader->Program, (line + "quadratic").c_str()), quadratic);
	}
};

class SunLight : public Light {
public:
	glm::vec3 direction;

	SunLight(glm::vec3 dir, glm::vec3 col) : direction(dir) {
		specular = color = col;
		diffuse = col * 0.5f;
		ambient = col * 0.05f;
	}
	void SetActive(Shader* shader) {
		shader->Use();
		glUniform3f(glGetUniformLocation(shader->Program, "Dlight.ambient"), ambient.x, ambient.y, ambient.z);
		glUniform3f(glGetUniformLocation(shader->Program, "Dlight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
		glUniform3f(glGetUniformLocation(shader->Program, "Dlight.specular"), specular.x, specular.y, specular.z);
											   
		glUniform3f(glGetUniformLocation(shader->Program, "Dlight.direction"), direction.x, direction.y, direction.z);
	}
};

class FlashLight : Light {
public:
	bool active = false;
	glm::vec3 direction;
	glm::vec3 position;
	float cutOff = 56.5f;
	float outerCutOff = 65.5f;
	FlashLight(glm::vec3 dir, glm::vec3 pos, glm::vec3 col) : direction(dir) {
		specular = color = col;
		diffuse = col * 0.9f;
		ambient = col * 0.f;
	}

	void SetAttenuation(float cnst, float lin, float qdc) {
		constant = cnst;
		linear = lin;
		quadratic = qdc;
	}

	void Refresh(glm::vec3 dir, glm::vec3 pos) {
		direction = dir;
		position = pos;
	}

	void SetActive(Shader* shader, bool active) {
		shader->Use();
		glUniform1i(glGetUniformLocation(shader->Program, "Flight.active2"), active);
		glUniform3f(glGetUniformLocation(shader->Program, "Flight.ambient"), ambient.x, ambient.y, ambient.z);
		glUniform3f(glGetUniformLocation(shader->Program, "Flight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
		glUniform3f(glGetUniformLocation(shader->Program, "Flight.specular"), specular.x, specular.y, specular.z);
		glUniform3f(glGetUniformLocation(shader->Program, "Flight.position"), position.x, position.y, position.z);   //cameraPos.x, cameraPos.y, cameraPos.z
		glUniform1f(glGetUniformLocation(shader->Program, "Flight.constant"), constant);
		glUniform1f(glGetUniformLocation(shader->Program, "Flight.linear"), linear);
		glUniform1f(glGetUniformLocation(shader->Program, "Flight.quadratic"), quadratic);
		glUniform3f(glGetUniformLocation(shader->Program, "Flight.direction"), direction.x, direction.y, direction.z); //normCursore.x, normCursore.y, normCursore.z
		glUniform1f(glGetUniformLocation(shader->Program, "Flight.cutOff"), glm::cos(glm::radians(cutOff)));
		glUniform1f(glGetUniformLocation(shader->Program, "Flight.outerCutOff"), glm::cos(glm::radians(outerCutOff)));
	}
};
