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

layout(location = 0) out vec4 Frag_color;
layout(location = 1) out vec4 Bright_color;
uniform sampler2D ScreenTexture;
uniform float brightnessLine;
in vec2 texPos;	

void main(){
	Frag_color = texture(ScreenTexture, texPos);
	float brightness = dot(Frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness>brightnessLine)
		Bright_color = vec4(Frag_color.rgb, 1.0);
	else 
		Bright_color = vec4(0.0, 0.0, 0.0, 1.0);
}