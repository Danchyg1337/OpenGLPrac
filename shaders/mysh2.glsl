#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture;
layout(location = 2) in vec3 normal;
out vec3 Normal;
out vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 objColor;
uniform mat4 projection;
uniform mat4 model;

void main(){
	Normal = normal;
	vec3 npos = (position * -1);
	FragPos = vec3(model * vec4(npos, 1.0));
	gl_Position =  projection  * vec4(npos, 1.0f);
};

#shader fragment
#version 330 core
in vec3 Normal;
in vec3 FragPos;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objColor;
out vec4 Frag_color;
void main(){
	float ambientPow = 0.1f;
	float specularPow = 0.5f;
	vec3 ambient = ambientPow*lightColor;
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(FragPos - lightPos);
	vec3 viewDir = normalize(FragPos - viewPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float diff = max(dot(norm, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 diffuse = diff * lightColor;
	vec3 specular = specularPow * spec * lightColor;
	vec3 res = (ambient + diffuse + specular) * objColor;
	Frag_color = vec4(res, 1.0);
};