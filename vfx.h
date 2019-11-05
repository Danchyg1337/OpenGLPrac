#pragma once
class VFX {
public:
    VFX(const VFX&) = delete;
    VFX& operator=(VFX&) = delete;
    static VFX* GetInstance();

    void BlurBrightAreas(GLuint screenTexture, GLuint dstTexture, float brightness, unsigned weightNum, float gaussianFloat, int blurAmount, float* levels, unsigned levelsNum);
    void BlurTexture(GLuint srcTexture, GLuint dstTexture, unsigned weightNum, float gaussianFloat, int blurAmount, float* levels, unsigned levelsNum);
private:
    static VFX* Instance;
    VFX() {};
    void Init(unsigned width, unsigned height);
    float BlurGaussianFunction(float X, float A);
    float* blurWeight;
    float blurGaussianFloat;
    unsigned blurWeightNum = 5, blurTwidth = 0, blurTheight = 0;
    bool Initialized = false;
    Shader* shaderBlur;
    Shader* shader2Buffers;
	Shader* shader2Buffers2;
    GLuint ppfbo[2], pptbo[2], fboOut;

    unsigned int quadVAO, quadVBO;
    GLuint fboDouble, rboDouble, tboDouble[2];
};
VFX* VFX::Instance = nullptr;
