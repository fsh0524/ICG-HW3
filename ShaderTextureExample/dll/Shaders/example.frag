#version 450

layout(binding = 0) uniform sampler2D diffuseMap;

in vec2 texcoord;
out vec4 outColor;

void main(){
	outColor = texture2D(diffuseMap, texcoord);
}