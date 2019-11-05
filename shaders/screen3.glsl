#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texturePos;

out vec2 texPos;
void main(){
	texPos = texturePos;
	gl_Position = vec4(position, 0.0, 1.0);
}

#shader fragment
#version 330 core
uniform sampler2D BlurTexture;
uniform sampler2D ColorTexture;
uniform float level;
out vec4 Frag_color;
in vec2 texPos;	

void main(){
	Frag_color = texture(ColorTexture, texPos) + texture(BlurTexture, texPos);
}