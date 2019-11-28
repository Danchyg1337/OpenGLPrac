#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texturePos;

out vec2 textureCor;
void main(){
	textureCor = texturePos;
	gl_Position = vec4(position, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 Frag_color;
in vec2 textureCor;
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

uniform samplerCube cubemap;
uniform float refraction;

#define NUM_LIGHTS 5

uniform PointLight Plight[NUM_LIGHTS];
uniform DirLight Dlight;
uniform FlashLight Flight;

uniform float shininess;

uniform vec3 viewPos;

uniform sampler2D textureGc;
uniform sampler2D textureGn;
uniform sampler2D textureGp;

vec3 CalcPointLight(PointLight plight, vec3 normal, vec3 fragPos, vec3 DirView);
vec3 CalcDirLight(DirLight dlight, vec3 normal, vec3 DirView);
vec3 CalcFlashLight(FlashLight flight, vec3 normal, vec3 fragPos, vec3 DirView);
vec3 CalcReflection(vec3 viewPos, vec3 normal, vec3 FragPos, samplerCube cubemap);


void main(){
	vec3 FragPos = texture(textureGp, textureCor).rgb;
	vec3 normal = texture(textureGn, textureCor).rgb;
	vec4 colorAS = texture(textureGc, textureCor);

	vec3 viewDir = normalize(FragPos - viewPos);
	
	vec3 res; 
	res += CalcDirLight(Dlight, normal, viewDir);
	for(int t=0;t<NUM_LIGHTS;t++){
		res += CalcPointLight(Plight[t], normal, FragPos, viewDir);
	}
	if(Flight.active2) res += CalcFlashLight(Flight, normal, FragPos, viewDir);
	float intens = colorAS.a;
	if(intens > 0.05) res += CalcReflection(viewPos, normal, FragPos, cubemap) * intens;

	Frag_color = vec4(res, 1.);
	
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
	float spec = pow(max(dot(-DirView, reflectDir), 0.0), shininess);

	vec3 ambient = dlight.ambient * vec3(texture(textureGc, textureCor));
	vec3 diffuse = diff * vec3(texture(textureGc, textureCor)) * dlight.diffuse;
	vec3 specular = spec * vec3(texture(textureGc, textureCor).a) * dlight.specular;

	return(ambient + diffuse + specular);
};

vec3 CalcPointLight(PointLight plight, vec3 normal, vec3 fragPos, vec3 viewDir){
	
	vec3 lightDir = normalize(plight.position - fragPos);

	float distance = length(fragPos - plight.position);
	float attenuation = 1.0f/(plight.constant+plight.linear*distance+plight.quadratic*(distance*distance));

	vec3 reflectDir = reflect(lightDir, normal);
	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 ambient = plight.ambient * vec3(texture(textureGc, textureCor));
	vec3 diffuse = diff * vec3(texture(textureGc, textureCor)) * plight.diffuse;
	vec3 specular = spec * vec3(texture(textureGc, textureCor).a) * plight.specular;

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

		float distance = length(flight.position - fragPos);
		float attenuation = 1.0f/(flight.constant+flight.linear*distance+flight.quadratic*(distance*distance));

		vec3 res = CalcPointLight(pl, normal, fragPos, viewDir);
		return(res*intensity*attenuation);
	}
	return(flight.ambient * vec3(texture(textureGc, textureCor)));
};