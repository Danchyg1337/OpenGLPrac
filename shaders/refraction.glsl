#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
out vec3 fragPos;
out vec3 normalL;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main(){
	normalL = mat3(transpose(inverse(model))) * normal; 
	fragPos = vec3(model * vec4(position, 1.0));
	gl_Position =  view * model * vec4(position, 1.0);
}

#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out;
uniform mat4 projection;
uniform float time;
in vec3 fragPos[];
in vec3 normalL[];
out vec3 FragPos;
out vec3 Normal;

vec3 GetNormal(){
	vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void main(){
	float magnitude = ((sin(time) + 1.0) / 2.0);

	vec3 normal = GetNormal();

	for(int t=0;t<3;t++){
		gl_Position = projection * (gl_in[t].gl_Position - (vec4(normal, 0.0) * 0.0));
		Normal = normalL[t];
		FragPos = fragPos[t] + (normal * 0.0);
		EmitVertex();
	}
	EndPrimitive();

};

#shader fragment
#version 330 core
uniform samplerCube cubemap;
uniform vec3 viewPos;
//uniform vec3 color;
uniform float refraction;
out vec4 Frag_color;
in vec3 Normal;
in vec3 FragPos;
void main(){
	vec4 col = vec4(1);
	float ratio = 1.00/refraction;
	vec3 dir = normalize(FragPos - viewPos);
	vec3 reflectDir = refract(dir, normalize(Normal), ratio);
	Frag_color = texture(cubemap, reflectDir) * col;
}