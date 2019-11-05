#shader vertex
#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main(){
	vec3 FragPos = vec3(model * vec4(position, 1.0));
	gl_Position =  projection * view * model * vec4(position, 1.0f);
};

#shader fragment
#version 330 core
out vec4 Frag_color;
uniform vec3 lightColor;
void main(){
	Frag_color = vec4(lightColor, 1.0f);
};