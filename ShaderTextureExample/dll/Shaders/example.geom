#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 geom_pos[3];
in vec3 geom_norm[3];
in vec2 geom_tex[3];

out vec2 texcoord;
out vec3 view;
out vec3 light;
out vec3 frag_normal;
out vec3 frag_tangent;
out vec3 frag_bitangent;

uniform vec3 viewPos;
uniform vec3 lightPos;

void main() {
	vec3 edge1 = geom_pos[1] - geom_pos[0];
	vec3 edge2 = geom_pos[2] - geom_pos[0];
	vec2 deltaUV1 = geom_tex[1] - geom_tex[0];
	vec2 deltaUV2 = geom_tex[2] - geom_tex[0];

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	
	frag_tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	frag_tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	frag_tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	frag_tangent = normalize(frag_tangent);

	for (int i = 0 ; i < gl_in.length() ; ++i) {
		gl_Position = gl_in[i].gl_Position;
		texcoord = geom_tex[i];
		frag_normal = geom_norm[i];
		frag_bitangent = cross(frag_normal, frag_tangent);
		light = normalize(lightPos - geom_pos[i]);
		view = normalize(viewPos - geom_pos[i]);
		EmitVertex();
	}
	EndPrimitive();
}