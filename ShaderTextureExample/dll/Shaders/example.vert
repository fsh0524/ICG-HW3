#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

out vec3 geom_pos;
out vec3 geom_norm;
out vec2 geom_tex;

uniform mat4 Projection;
uniform mat4 model;

void main() {
	gl_Position = Projection * model * vec4(pos, 1.0);
	
	geom_pos = vec3(model * vec4(pos, 1.0));
	geom_norm = normalize(normal);
	geom_tex = texcoord;
	
}