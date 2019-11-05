#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
out vec3 textureDir;
uniform mat4 projection;
uniform mat4 view;
void main(){
	textureDir = aPos;
	vec4 pos = projection * view * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}
#shader fragment
#version 330 core
in vec3 textureDir;
uniform samplerCube cubemap;
out vec4 Frag_color;
void main(){
	Frag_color = texture(cubemap, textureDir);
}