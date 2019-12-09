#pragma once
Scene::Scene(GLuint screenWidth, GLuint screenHeight) {
	this->screenShader = new Shader("shaders/screenDefault.glsl");
	this->shaderNormals = new Shader("shaders/showNormals.glsl");
	this->shaderGBuffer = new Shader("shaders/GBuffer.glsl");
	this->shaderGBufferShow = new Shader("shaders/showGBuffer.glsl");
	this->shaderGBufferDefaultShading = new Shader("shaders/GBshading.glsl");
	this->width = screenWidth;
	this->height = screenHeight;
	glGenFramebuffers(1, &fboHDR);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
	glGenRenderbuffers(1, &rboHDR);
	glBindRenderbuffer(GL_RENDERBUFFER, rboHDR);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboHDR);

	glGenTextures(1, &tboHDR);
	glBindTexture(GL_TEXTURE_2D, tboHDR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tboHDR, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER::NOTCOMPLETE\n";
	}
	////
	GLuint quadVBO;
	glGenVertexArrays(1, &screenQuadVao);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(screenQuadVao);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenVertices), &ScreenVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	glGenFramebuffers(1, &fboG);
	glGenRenderbuffers(1, &rboG);
	glGenTextures(1, &tboGc);
	glGenTextures(1, &tboGn);
	glGenTextures(1, &tboGp);
	glBindFramebuffer(GL_FRAMEBUFFER, fboG);
	glBindRenderbuffer(GL_RENDERBUFFER, rboG);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboG);
	glBindTexture(GL_TEXTURE_2D, tboGc);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tboGc, 0);
	glBindTexture(GL_TEXTURE_2D, tboGn);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tboGn, 0);
	glBindTexture(GL_TEXTURE_2D, tboGp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tboGp, 0);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	sphere = new Model();
	sphere->path = "models/sphere/sphere.obj";
	sphere->shader = shaderGBufferDefaultShading;
	sphere->EnableCullFace(GL_CW);
	sphere->loadModel(sphere->path.value());
}

GLuint Scene::AddModel(Model* model) {
	models.push_back(model);
	return models.size() - 1;
}

void Scene::LoadModels() {
	for (Model* c : models) {
		//v_futures.push_back(std::async(std::launch::async, &Scene::LoadSingleModel, this, c));
		LoadSingleModel(c);
	}
}

void Scene::LoadSingleModel(Model* mdl) {
	//std::lock_guard<std::mutex> lock(mut_modelsLoad);
	//std::cout << "Loading " << mdl->path.value_or("NONE") << std::endl;
	if(mdl->path.has_value())
		mdl->loadModel(mdl->path.value());
	
	if (mdl->transparent) TrModels.push_back(mdl);
	else simpleModels.push_back(mdl);
	//std::cout << "Loaded " << mdl->path.value_or("NONE ") << " with " <<mdl->meshes[0].verts.size()<<std::endl;
}

bool Scene::DrawToFramebuffer(GLuint frmBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frmBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER::NOTCOMPLETE\n";
		return false;
	}
	this->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

bool Scene::DeleteModel(GLuint &id) {
	try {
		delete models[id];
		delete &id;
	}
	catch (...) {
		return 0;
	}
	return 1;
}

void Scene::Draw(Shader* toAllModels) {
	//std::lock_guard<std::mutex> lock(mut_modelsLoad);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	SortTransparent();
	//draw not transparent
	for (Model* mdl : simpleModels) {
		if (mdl->cullFace) {
			glEnable(GL_CULL_FACE);
			glCullFace(mdl->cullFaceType);
		}
		mdl->shader->Use();
		if (mdl->cubemap.has_value()) (*mdl->cubemap)->Bind();
		for (int t = 0; t < PointLights.size(); t++) {
			PointLights[t]->SetActive(mdl->shader, t);
		}
		if (sun.has_value()) (*sun)->SetActive(mdl->shader);
		if (flashlight.has_value()) {
			if ((*flashlight)->active) (*flashlight)->Refresh(CameraDirection, CameraPosition);
			(*flashlight)->SetActive(mdl->shader, (*flashlight)->active);
		}
		SetUniforms(mdl);
		mdl->Draw();
		(*mdl->cubemap)->Unbind();
		if (showNormals) ShowNormals(mdl);
		glDisable(GL_CULL_FACE);
	}

	//draw cubemap
	skybox->Bind();
	cubemapShader->Use();
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glBindVertexArray(skybox->vao_id);
	glUniformMatrix4fv(cubemapShader->SetUniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(cubemapShader->SetUniform("view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(view))));
	glUniform1i(cubemapShader->SetUniform(		 "cubemap"), 8);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	skybox->Unbind();

	//draw transparent 
	glEnable(GL_BLEND);
	for (std::map<float, Model*>::reverse_iterator it = transparentModels.rbegin(); it != transparentModels.rend(); it++) {
		it->second->shader->Use();
		for (int t = 0; t < 5; t++) {
			PointLights[t]->SetActive(it->second->shader, t);
		}
		if (sun.has_value()) (*sun)->SetActive(it->second->shader);
		SetUniforms(it->second);
		it->second->Draw();
		glUseProgram(0);
	}
	glDisable(GL_BLEND);
}

void Scene::DrawToScreen_Shader(Shader* shader) {
	glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
	this->Draw();
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(screenQuadVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tboHDR);
	shader->Use();
	glUniform1i(shader->SetUniform("ScreenTexture"), 0);
	glUniform1f(shader->SetUniform("gamma"), this->gamma);
	glUniform1f(shader->SetUniform("exposure"), this->exposure);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
}

void Scene::DrawToScreen_Texture(GLuint* texture) {
	//glViewport(0, 0, this->width, this->height);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(screenQuadVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);
	screenShader->Use();
	glUniform1i(screenShader->SetUniform("ScreenTexture"), 0);
	glUniform1f(screenShader->SetUniform("gamma"), this->gamma);
	glUniform1f(screenShader->SetUniform("exposure"), this->exposure);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Scene::DrawToScreen() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
	this->Draw();
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(screenQuadVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tboHDR);
	screenShader->Use();
	glUniform1i(screenShader->SetUniform("ScreenTexture"), 0);
	glUniform1f(screenShader->SetUniform("gamma"), this->gamma);
	glUniform1f(screenShader->SetUniform("exposure"), this->exposure);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::Clear() {
	for (auto& a : models) {
		delete a;
	}
	models.clear();
	for (auto& a : PointLights) {
		delete a;
	}
	PointLights.clear();
}

void Scene::SetUniforms(Model *mdl, Shader* custom) {
	Shader* sh;
	if (custom != nullptr) sh = custom;
	else sh = mdl->shader;
	glUniform3f(sh->SetUniform("lightColor"), (*mdl->lightColor).x, (*mdl->lightColor).y, (*mdl->lightColor).z);
	glUniformMatrix4fv(sh->SetUniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(sh->SetUniform("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(sh->SetUniform("model"), 1, GL_FALSE, glm::value_ptr(mdl->model));
	glUniform3f(sh->SetUniform("viewPos"), CameraPosition.x, CameraPosition.y, CameraPosition.z);
	glUniform1f(sh->SetUniform("material.shininess"), 4.0f);
	glUniform1i(sh->SetUniform("cubemap"), 8);
	glUniform1f(sh->SetUniform("refraction"), 1.31);
	glUniform1f(sh->SetUniform("time"), glfwGetTime());
}


void Scene::SortTransparent() {
	transparentModels.clear();
	for (Model* a : TrModels) {
		transparentModels[glm::length(a->position - CameraPosition)] = a;						
	}
}

void Scene::ShowNormals(Model* mdl) {
	this->shaderNormals->Use();
	glUniformMatrix4fv(mdl->shader->SetUniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(mdl->shader->SetUniform("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(mdl->shader->SetUniform("model"), 1, GL_FALSE, glm::value_ptr(mdl->model));

	mdl->DrawShader(shaderNormals);
	glUseProgram(0);
}

void Scene::UpdateModels() {
	for (auto mdl : simpleModels) {
		mdl->model = glm::translate(glm::mat4(1.0f), mdl->position);
		mdl->model = glm::rotate(mdl->model, glm::radians(mdl->rotation.x), glm::vec3(1, 0, 0));
		mdl->model = glm::rotate(mdl->model, glm::radians(mdl->rotation.y), glm::vec3(0, 1, 0));
		mdl->model = glm::rotate(mdl->model, glm::radians(mdl->rotation.z), glm::vec3(0, 0, 1));
		mdl->model = glm::scale(mdl->model, mdl->scale);
	}
	for (auto mdl : TrModels)
		mdl->model = glm::translate(glm::mat4(1.0f), mdl->position);
}

GLuint Scene::FindModel(Model* mdl)
{
	size_t ret = 0;
	for (auto t : models) {
		if (mdl == t) return ret;
		ret++;
	}
};
void PrintErrors();
void Scene::DrawToGBuffer_Screen() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboG);
	GLuint drawbuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, drawbuffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	SortTransparent();
	//draw not transparent
	for (Model* mdl : simpleModels) {
		if (mdl->lightColor.has_value()) break;
		if (mdl->cullFace) {
			glEnable(GL_CULL_FACE);
			glCullFace(mdl->cullFaceType);
		}
		shaderGBuffer->Use();
		if (mdl->cubemap.has_value()) (*mdl->cubemap)->Bind();
		SetUniforms(mdl, shaderGBuffer);
		mdl->DrawShader(shaderGBuffer);
		(*mdl->cubemap)->Unbind();
		//if (showNormals) ShowNormals(mdl);
		glDisable(GL_CULL_FACE);
	}

	//draw transparent 
	//glEnable(GL_BLEND);
	for (std::map<float, Model*>::reverse_iterator it = transparentModels.rbegin(); it != transparentModels.rend(); it++) {
		shaderGBuffer->Use();
		SetUniforms(it->second, shaderGBuffer);
		it->second->DrawShader(shaderGBuffer);
		glUseProgram(0);
	}
	glDisable(GL_BLEND);


	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//glBindVertexArray(screenQuadVao);
	//shaderGBufferShow->Use();
	shaderGBufferDefaultShading->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tboGc);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tboGn);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tboGp);
	glUniform1i(shaderGBufferDefaultShading->SetUniform("textureGc"), 0);
	glUniform1i(shaderGBufferDefaultShading->SetUniform("textureGn"), 1);
	glUniform1i(shaderGBufferDefaultShading->SetUniform("textureGp"), 2);
	glUniform3f(shaderGBufferDefaultShading->SetUniform("viewPos"), CameraPosition.x, CameraPosition.y, CameraPosition.z);
	glUniform1f(shaderGBufferDefaultShading->SetUniform("shininess"), 4.0f);
	glUniform1i(shaderGBufferDefaultShading->SetUniform("cubemap"), 8);
	glUniform1f(shaderGBufferDefaultShading->SetUniform("refraction"), 1.31);
	glUniform1f(shaderGBufferDefaultShading->SetUniform("time"), glfwGetTime());
	char udata[50];
	for (int t = 0; t < PointLights.size(); t++) {
		PointLights[t]->SetActive(shaderGBufferDefaultShading, t);
		sprintf_s(udata, "model[%d]", t);
		glUniformMatrix4fv(glGetUniformLocation(shaderGBufferDefaultShading->Program, udata), 1, GL_FALSE, glm::value_ptr(glm::scale(PointLights[t]->modelID.value()->model, glm::vec3(PointLights[t]->radius)))); 
	}
	if (sun.has_value()) (*sun)->SetActive(shaderGBufferDefaultShading);
	if (flashlight.has_value()) {
		if ((*flashlight)->active) (*flashlight)->Refresh(CameraDirection, CameraPosition);
		(*flashlight)->SetActive(shaderGBufferDefaultShading, (*flashlight)->active);
	}

	glUniformMatrix4fv(glGetUniformLocation(shaderGBufferDefaultShading->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderGBufferDefaultShading->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);							
	glBlendEquation(GL_FUNC_ADD);	
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	sphere->DrawInstanced(PointLights.size());
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);


	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboG);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHDR);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
	for(auto mdl : PointLights)
		if (mdl->modelID.has_value()) {
			(*mdl->modelID)->shader->Use();
			if ((*mdl->modelID)->cullFace) {
				glEnable(GL_CULL_FACE);
				glCullFace((*mdl->modelID)->cullFaceType);
			}
			SetUniforms((*mdl->modelID));
			(*mdl->modelID)->Draw();
			glDisable(GL_CULL_FACE);
		}

	skybox->Bind();
	cubemapShader->Use();
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glBindVertexArray(skybox->vao_id);
	glUniformMatrix4fv(cubemapShader->SetUniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(cubemapShader->SetUniform("view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(view))));
	glUniform1i(cubemapShader->SetUniform("cubemap"), 8);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	skybox->Unbind();
	
	DrawToScreen_Texture(&tboHDR);
}