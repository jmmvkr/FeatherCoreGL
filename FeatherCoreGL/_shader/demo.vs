#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 vertexColor;
out vec2 TexCoord;
out vec3 outPos;
out vec3 outNormal;

uniform mat4 model;
uniform mat4 pv;

void main(){
	vec4 mpos = model * vec4(aPos, 1.0);
    gl_Position = pv * mpos;
	outPos = mpos.xyz;
	outNormal = aNormal;

    vertexColor = vec3(1.0f, 1.f, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}