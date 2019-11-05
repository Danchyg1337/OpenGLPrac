#shader vertex
#version 330 core
layout(location = 0) in vec3 position;

void main(){
	gl_Position =  vec4(position, 1.0);
};

#shader fragment
#version 330 core
out vec4 Frag_color;
uniform vec3 color;
void main(){
	Frag_color = vec4(color, 1.0);
};