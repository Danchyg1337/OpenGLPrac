#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;


out vec3 fragPos;
out vec3 Normal;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main(){
	Normal = normal;
	fragPos = vec3(model * vec4(position, 1.0));
	gl_Position = projection * view * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core

in vec3 fragPos;
in vec3 Normal;

out vec4 Frag_color;

uniform samplerCube cubemap;
uniform vec3 viewPos;

void main(){
	vec3 dir = fragPos - viewPos;
	vec3 refr = refract(dir, Normal, 1.0/3.11);
	Frag_color = texture(cubemap, refr);
}