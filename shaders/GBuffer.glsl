#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture;
out vec3 Normal;
out vec3 FragPos;
out vec2 textureCor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
	Normal = mat3(transpose(inverse(model))) * normal;
	FragPos = vec3(model * vec4(position, 1.0));
	gl_Position = projection * view * model * vec4(position, 1.0f);
	textureCor = vec2(texture.x, texture.y);
};

#shader fragment
#version 330 core
layout(location = 0) out vec3 Gposition;
layout(location = 1) out vec3 Gnormal;
layout(location = 2) out vec4 Gcolor;

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};

uniform float level;
out vec4 Frag_color;
in vec3 Normal;
in vec2 textureCor;	
in vec3 FragPos;

uniform Material material; 

void main(){
	Gposition = FragPos;
	Gnormal = Normal;
	Gcolor.rgb = texture(material.texture_diffuse1, textureCor).rgb;
	Gcolor.a = texture(material.texture_specular1, textureCor).r;
}