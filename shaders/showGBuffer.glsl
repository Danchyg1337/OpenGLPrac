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
uniform sampler2D textureGc;
uniform sampler2D textureGn;
uniform sampler2D textureGp;
out vec4 Frag_color;
in vec2 texPos;	

void main(){
	vec2 ts = textureSize(textureGc, 0)*.5;
	if(gl_FragCoord.x<ts.x && gl_FragCoord.y>ts.y)
		Frag_color = vec4(texture(textureGc, texPos*2.).rgb, 1.);
	if(gl_FragCoord.x>ts.x && gl_FragCoord.y<ts.y)
		Frag_color = texture(textureGc, texPos*2.).aaaa;
	if(gl_FragCoord.x>ts.x && gl_FragCoord.y>ts.y)
		Frag_color = texture(textureGn, texPos*2.);
	if(gl_FragCoord.x<ts.x && gl_FragCoord.y<ts.y)
		Frag_color = texture(textureGp, texPos*2.);
}