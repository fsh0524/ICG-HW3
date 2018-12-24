#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 geom_pos[3];
in vec3 geom_norm[3];
in vec2 geom_tex[3];

out vec3 normal;
out vec3 tangent;
out vec3 bitangent;
out vec2 texcoord;
out vec3 view;
out vec3 light;

uniform vec3 viewPos;
uniform vec3 lightPos;

void main() {
	vec3 edge1 = geom_pos[1] - geom_pos[0];
	vec3 edge2 = geom_pos[2] - geom_pos[0];
	vec2 deltaUV1 = geom_tex[1] - geom_tex[0];
	vec2 deltaUV2 = geom_tex[2] - geom_tex[0];

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = normalize(bitangent);

	for (int i = 0 ; i < gl_in.length() ; ++i) {
		gl_Position = gl_in[i].gl_Position;
		texcoord = geom_tex[i];
		normal = geom_norm[i];
		view = normalize(viewPos - geom_pos[i]);
		light = normalize(lightPos - geom_pos[i]);
		EmitVertex();
	}
	EndPrimitive();
}