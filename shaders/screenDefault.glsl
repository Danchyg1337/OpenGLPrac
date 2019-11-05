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
uniform sampler2D ScreenTexture;
uniform float gamma = 1.0;
uniform float exposure = 2.2;
in vec2 texPos;	
out vec4 Frag_color;

void main(){
	vec3 hdrColor = texture(ScreenTexture, texPos).rgb;

	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	mapped = pow(mapped, vec3(1.0/gamma));

	Frag_color = vec4(mapped, 1.0);
}