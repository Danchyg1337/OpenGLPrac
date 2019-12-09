#shader vertex
#version 330 core
layout(location = 0) in vec3 position;

struct PointLight{
	vec3 position;
	float radius;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NUM_LIGHTS 5
uniform PointLight Plight[NUM_LIGHTS];
out PointLight plight;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model[NUM_LIGHTS];

void main(){
	float r = Plight[gl_InstanceID].radius;
	vec3 p = Plight[gl_InstanceID].position;
	gl_Position = projection*view*model[gl_InstanceID]*vec4(position, 1.0);
	plight = Plight[gl_InstanceID];
}

#shader fragment
#version 330 core
out vec4 Frag_color;
struct PointLight{
	vec3 position;
	float radius;
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


in PointLight plight;
uniform DirLight Dlight;
uniform FlashLight Flight;

uniform float shininess;

uniform vec3 viewPos;

uniform sampler2D textureGc;
uniform sampler2D textureGn;
uniform sampler2D textureGp;

vec3 CalcPointLight(PointLight plight, vec3 normal, vec3 fragPos, vec3 DirView);
vec3 CalcReflection(vec3 viewPos, vec3 normal, vec3 FragPos, samplerCube cubemap);
vec2 textureCor;

void main(){
	textureCor = gl_FragCoord.xy / textureSize(textureGp, 0).xy;
	vec3 FragPos = texture(textureGp, textureCor).rgb;
	vec3 normal = texture(textureGn, textureCor).rgb;
	vec4 colorAS = texture(textureGc, textureCor);

	vec3 viewDir = normalize(FragPos - viewPos);
	
	vec3 res = CalcPointLight(plight, normal, FragPos, viewDir);
	float intens = colorAS.a;
	if(intens > 0.05) res += CalcReflection(viewPos, normal, FragPos, cubemap) * intens;

	Frag_color = vec4(res, 1.);
	
};

vec3 CalcReflection(vec3 viewPos, vec3 normal, vec3 FragPos, samplerCube cubemap){
	vec3 dir = normalize(viewPos - FragPos);
	vec3 reflectDir = normalize(reflect(-dir, normal));
	return vec3(texture(cubemap, reflectDir));
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

	return(diffuse + specular);
};