#pragma once
VFX* VFX::GetInstance(){
    if(!Instance)
        Instance = new VFX();
    return Instance;
}

void VFX::BlurTexture(GLuint srcTexture, GLuint dstTexture, unsigned weightNum, float gaussianFloat, int blurAmount, float* levels, unsigned levelsNum){
    int w1, h1, w2, h2;
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w1);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h1);
    glBindTexture(GL_TEXTURE_2D, dstTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w2);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h2);

    if((w1!=w2)||(h1!=h2)){
        std::cout<<"BLUR::ERROR::DIFFERENT_TEXTURE_SIZE\n";
        return;
    }
    if(!Initialized) Init(w1, h1);
    if((weightNum!=blurWeightNum)||gaussianFloat!=blurGaussianFloat){
        blurWeight = new float[weightNum];
        float step = (gaussianFloat + 2*gaussianFloat)/weightNum;
        for(int t=0;t<weightNum;t++){
            *(blurWeight+t) = BlurGaussianFunction(t*step, gaussianFloat);
        }
        blurWeightNum = weightNum;
        blurGaussianFloat = gaussianFloat;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboOut);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
    
    if((w1!=blurTwidth)||(h1!=blurTheight)){
        glTextureImage2DEXT(pptbo[0], GL_TEXTURE_2D, 0, GL_RGB16F, w1, h1, 0, GL_RGBA, GL_FLOAT, NULL);
        glTextureImage2DEXT(pptbo[1], GL_TEXTURE_2D, 0, GL_RGB16F, w1, h1, 0, GL_RGBA, GL_FLOAT, NULL);
        blurTwidth = w1;
        blurTheight = h1;
    }

    glViewport(0, 0, w1, h1);
    shaderBlur->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    glUniform1i( shaderBlur->SetUniform("ScreenTexture"), 0);
    glUniform1i( shaderBlur->SetUniform("kSize"), weightNum);
    glUniform1fv(shaderBlur->SetUniform("weight"), weightNum, blurWeight);
	glUniform1i(shaderBlur->SetUniform("levelsNum"), levelsNum);
	glUniform1fv(shaderBlur->SetUniform("level"), levelsNum, levels);
  	int horizontal = true;
   	bool first = true;
	for (int t = 0; t < blurAmount; t++) {
		glGenerateTextureMipmap(pptbo[horizontal]);
		if(t==blurAmount-1) glBindFramebuffer(GL_FRAMEBUFFER, fboOut);
		else glBindFramebuffer(GL_FRAMEBUFFER, ppfbo[horizontal]);
		glBindTexture(GL_TEXTURE_2D, first ? srcTexture : pptbo[!horizontal]);
		glUniform1i(shaderBlur->SetUniform("horizontal"), horizontal);
		horizontal = !horizontal;
		if (first) first = false;
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glTextureSubImage2D(pptbo[0], 0, 0, 0, w1, h1, GL_RGB16F, GL_FLOAT, 0);
	glTextureSubImage2D(pptbo[1], 0, 0, 0, w1, h1, GL_RGB16F, GL_FLOAT, 0);
}
float VFX::BlurGaussianFunction(float X, float A){
    return 1/(A*sqrt(2*M_PI))*std::exp((-X*X)/(2*A*A));
}
void VFX::Init(unsigned width, unsigned height){
	glGenFramebuffers(2, ppfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, ppfbo[0]);

	glGenTextures(2, pptbo);
	for (int t = 0; t < 2; t++) {
		glBindFramebuffer(GL_FRAMEBUFFER, ppfbo[t]);
		glBindTexture(GL_TEXTURE_2D, pptbo[t]);
		glTexStorage2D(GL_TEXTURE_2D, 10, GL_RGB16F, width, height);
		glGenerateMipmap(GL_TEXTURE_2D); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pptbo[t], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER::NOTCOMPLETE" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    glGenFramebuffers(1, &fboOut);


	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(prmtvs::square), &prmtvs::square, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);


	glGenFramebuffers(1, &fboDouble);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDouble);

	glGenRenderbuffers(1, &rboDouble);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDouble);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDouble);

	glGenTextures(2, tboDouble);
	for (int t = 0; t < 2; t++) {
		glBindTexture(GL_TEXTURE_2D, tboDouble[t]);
		glTexStorage2D(GL_TEXTURE_2D, 10, GL_RGB16F, width, height);
		glGenerateMipmap(GL_TEXTURE_2D); 
		glTextureParameteri(tboDouble[t], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(tboDouble[t], GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + t, GL_TEXTURE_2D, tboDouble[t], 0);
	}

    blurTwidth = width;
    blurTheight = height;

    shaderBlur = new Shader("shaders/screen.glsl");
    shader2Buffers = new Shader("shaders/screen2.glsl");
    shader2Buffers2 = new Shader("shaders/screen3.glsl");
    Initialized = true;
}

void VFX::BlurBrightAreas(GLuint screenTexture, GLuint dstTexture, float brightness, unsigned weightNum, float gaussianFloat, int blurAmount, float* levels, unsigned levelsNum){  
    int widthS, heightS;
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &widthS);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &heightS);
    if(!Initialized) Init(widthS, heightS);

    glDisable(GL_DEPTH_TEST);
	glBindVertexArray(quadVAO);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDouble);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	shader2Buffers->Use();
	glUniform1i(shader2Buffers->SetUniform("ScreenTexture"), 0);
	glUniform1f(shader2Buffers->SetUniform("brightnessLine"), brightness);
	GLenum dbuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, dbuffers);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);


	glGenerateTextureMipmap(tboDouble[1]);

	VFX::GetInstance()->BlurTexture(tboDouble[1], dstTexture, weightNum, gaussianFloat, blurAmount, levels, levelsNum);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fboOut);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);


	shader2Buffers2->Use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);																		
	glBindTexture(GL_TEXTURE_2D, dstTexture);
	glUniform1i(shader2Buffers2->SetUniform("BlurTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tboDouble[0]);
	glUniform1i(shader2Buffers2->SetUniform("ColorTexture"), 1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}