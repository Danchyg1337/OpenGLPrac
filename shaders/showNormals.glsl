#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
out vec3 normalL;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main(){
	normalL = normal; 
	gl_Position = vec4(position, 1.0);
}

#shader geometry
#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

uniform float time;
in vec3 fragPos[];
in vec3 normalL[];
out vec3 FragPos;
out vec3 Normal;
out vec3 color;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

vec3 GetNormal(){
	vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void GenerateLine(int index, vec4 dir){
	vec4 vertViewSpace = view * model * gl_in[index].gl_Position;
	gl_Position =  projection * vertViewSpace;
	color = vec3(0, 1, 0);
	EmitVertex();
	gl_Position = projection * (vertViewSpace + (dir * 0.1));
	color = vec3(1, 0, 0);
	EmitVertex();
	EndPrimitive();
}

void main(){
	vec3 normal = GetNormal();
	vec4 nrml = normalize(transpose(inverse(view * model)) * vec4(normalL[0], 0.0));
	for(int t=0;t<3;t++){
		GenerateLine(0, nrml);
	}
};

#shader fragment
#version 330 core
out vec4 Frag_color;
in vec3 color;
void main(){
	Frag_color =  vec4(color, 1);
}