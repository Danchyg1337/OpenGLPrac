#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
out vec3 FragPos;
out vec3 Normal;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main(){
	Normal = mat3(transpose(inverse(model))) * normal;
	FragPos = vec3(model * vec4(position, 1.0));
	gl_Position = projection * view * model * vec4(position, 1.0);
}
#shader fragment
#version 330 core
uniform samplerCube cubemap;
uniform vec3 viewPos;

out vec4 Frag_color;
in vec3 Normal;
in vec3 FragPos;
void main(){
	vec3 dir = normalize(viewPos - FragPos);
	vec3 reflectDir = normalize(reflect(-dir, Normal));
	Frag_color = texture(cubemap, reflectDir);
}