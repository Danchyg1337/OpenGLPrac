#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture;
out vec3 Normal;
out vec3 FragPos;
out vec2 textureCor;

uniform vec3 lightColor;
uniform vec3 objColor;
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
in vec2 textureCor;
in vec3 Normal;
in vec3 FragPos;

out vec4 Frag_color;

struct PointLight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirLight{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct FlashLight{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	bool active2;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	sampler2D texture_reflect1;
	sampler2D texture_reflect2;

	float shininess;
};

uniform samplerCube cubemap;
uniform float refraction;

#define NUM_LIGHTS 5

uniform Material material;
uniform PointLight Plight[NUM_LIGHTS];
uniform DirLight Dlight;
uniform FlashLight Flight;

uniform float visible;

uniform vec3 viewPos;

vec3 CalcPointLight(PointLight plight, vec3 normal, vec3 fragPos, vec3 DirView);
vec3 CalcDirLight(DirLight dlight, vec3 normal, vec3 DirView);
vec3 CalcFlashLight(FlashLight flight, vec3 normal, vec3 fragPos, vec3 DirView);
vec3 CalcReflection(vec3 viewPos, vec3 normal, vec3 FragPos, samplerCube cubemap);


void main(){
	vec4 checkAlp = texture(material.texture_diffuse1, textureCor);
	float transp = checkAlp.a;
	if(transp < 0.1)
		discard;

	vec3 norm = normalize(Normal);

	vec3 viewDir = normalize(FragPos - viewPos);
	
	vec3 res; 
	res += CalcDirLight(Dlight, norm, viewDir);
	for(int t=0;t<NUM_LIGHTS;t++){
		res += CalcPointLight(Plight[t], norm, FragPos, viewDir);
	}
	if(Flight.active2) res += CalcFlashLight(Flight, norm, FragPos, viewDir);
	float intens = texture(material.texture_reflect1, textureCor).r;
	if(intens > 0.05) res += CalcReflection(viewPos, norm, FragPos, cubemap) * intens;

	Frag_color = vec4(res, transp);
	
};

vec3 CalcReflection(vec3 viewPos, vec3 normal, vec3 FragPos, samplerCube cubemap){
	vec3 dir = normalize(viewPos - FragPos);
	vec3 reflectDir = normalize(reflect(-dir, normal));
	return vec3(texture(cubemap, reflectDir));
};

vec3 CalcDirLight(DirLight dlight, vec3 normal, vec3 DirView){
	vec3 lightDir = normalize(dlight.direction);

	vec3 reflectDir = reflect(lightDir, normal);
	float diff = max(dot(normal, -lightDir), 0.0);
	float spec = pow(max(dot(-DirView, reflectDir), 0.0), material.shininess);

	vec3 ambient = dlight.ambient * vec3(texture(material.texture_diffuse1, textureCor));
	vec3 diffuse = diff * vec3(texture(material.texture_diffuse1, textureCor)) * dlight.diffuse;
	vec3 specular = spec * vec3(texture(material.texture_specular1, textureCor)) * dlight.specular;

	return(ambient + diffuse + specular);
};

vec3 CalcPointLight(PointLight plight, vec3 normal, vec3 fragPos, vec3 viewDir){
	
	vec3 lightDir = normalize(plight.position - fragPos);

	float distance = length(fragPos - plight.position);
	float attenuation = 1.0f/(plight.constant+plight.linear*distance+plight.quadratic*(distance*distance));

	vec3 reflectDir = reflect(lightDir, normal);
	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 ambient = plight.ambient * vec3(texture(material.texture_diffuse1, textureCor));
	vec3 diffuse = diff * vec3(texture(material.texture_diffuse1, textureCor)) * plight.diffuse;
	vec3 specular = spec * vec3(texture(material.texture_specular1, textureCor)) * plight.specular;

	diffuse *= attenuation;
	specular *= attenuation;

	return(ambient + diffuse + specular);
};

vec3 CalcFlashLight(FlashLight flight, vec3 normal, vec3 fragPos, vec3 viewDir){
	
	vec3 lightDir = normalize(flight.position - fragPos);

	float theta = dot(-lightDir, normalize(flight.direction));
	float epsilon =  flight.cutOff - flight.outerCutOff;
	float intensity = clamp((theta - flight.cutOff)/epsilon, 0.0, 1.0);
	if(theta > flight.outerCutOff){
		PointLight pl;
		pl.ambient = flight.ambient;
		pl.diffuse = flight.diffuse;
		pl.specular = flight.specular;
		pl.position = flight.position;
		pl.constant = flight.constant;
		pl.linear = flight.linear;
		pl.quadratic = flight.quadratic;

		float distance = length(flight.position - FragPos);
		float attenuation = 1.0f/(flight.constant+flight.linear*distance+flight.quadratic*(distance*distance));

		vec3 res = CalcPointLight(pl, normal, fragPos, viewDir);
		return(res*intensity*attenuation);
	}
	return(flight.ambient * vec3(texture(material.texture_diffuse1, textureCor)));
};