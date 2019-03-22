#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

void main() {
	vs_out.FragPos = aPos;
	vs_out.Normal = aNormal;
	vs_out.TexCoords = aTexCoord;
	gl_Position = projection * view * model * vec4(aPos, 1.0);	
}