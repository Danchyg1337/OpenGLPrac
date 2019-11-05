#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texturePos;

out vec2 texPos;
void main(){
	texPos = texturePos;
	gl_Position = vec4(position, 0.0, 1.0);
}

#shader fragment
#version 330 core
const int MAX_WEIGHT_SIZE = 32;
const int MAX_LEVELS_NUM = 32;
uniform int kSize;
uniform int levelsNum;
uniform float weight[MAX_WEIGHT_SIZE];
uniform float level[MAX_LEVELS_NUM];
uniform sampler2D ScreenTexture;
uniform bool horizontal;

out vec4 Frag_color;
in vec2 texPos;	

vec3 BlurLevel(float level);

void main(){
	vec3 color = vec3(0.0);
	for(int t=0;t<levelsNum;t++){
		color += BlurLevel(level[t]);
	}
	Frag_color = vec4(color, 1.0);
}

vec3 BlurLevel(float level){
	//if(textureLod(ScreenTexture, texPos, level) == vec4(0.0, 0.0, 0.0, 1.0)) return vec3(0.0);
	vec2 offset = 1.0/textureSize(ScreenTexture, int(level));
	vec2 offsets[9] = vec2[](
		vec2(-offset.x, offset.y),
		vec2(0, offset.y),
		vec2(offset.x, offset.y),
		vec2(-offset.x, 0),
		vec2(0, 0),
		vec2(offset.x, 0),
		vec2(-offset.x, -offset.y),
		vec2(0, -offset.y),
		vec2(offset.x, -offset.y)
	);

	float kernel[9] = float[](
		0, 0, 0,
		0, 1, 0,
		0, 0, 0
	);

	//vec3 cls[9];
	//for(int t=0;t<9;t++){
	//	cls[t] = vec3(textureLod(ScreenTexture, texPos.xy + offsets[t], level));
	//}

	vec3 color = vec3(0.0);
	//for(int t=0;t<9;t++){
	//	color += cls[t]*kernel[t];
	//}

	color *= weight[0];
	if(horizontal){
		for(int t=1;t<kSize;t++){
			color += textureLod(ScreenTexture, texPos + vec2(offset.x * t, 0), level).rgb * weight[t];
			color += textureLod(ScreenTexture, texPos - vec2(offset.x * t, 0), level).rgb * weight[t];
		}
	}
	else{
		for(int t=1;t<kSize;t++){
			color += textureLod(ScreenTexture, texPos + vec2(0, offset.y * t), level).rgb * weight[t];
			color += textureLod(ScreenTexture, texPos - vec2(0, offset.y * t), level).rgb * weight[t];
		}
	}
	return color;
}