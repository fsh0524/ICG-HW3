#version 450

layout(binding = 0) uniform sampler2D textureMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D specularMap;

in vec2 texcoord;
in vec3 view;
in vec3 light;
in vec3 frag_normal;
in vec3 frag_tangent;
in vec3 frag_bitangent;

out vec4 outColor;

void main() {
	
	vec3 norm_color = texture(normalMap, texcoord).rgb;
	float norm_r = norm_color.x * 2.0 - 1.0;
	float norm_g = norm_color.y * 2.0 - 1.0;
	float norm_b = norm_color.z * (-2.0) + 1.0;
	vec3 normal = normalize(norm_r * frag_tangent + norm_g * frag_bitangent + norm_b * frag_normal);

	float ambient_k = 0.1;
	// vec3 ambient_color = texture(textureMap, texcoord).rgb;
	vec3 ambient_color = vec3(0.7, 0.7, 0.7);
	vec3 ambient = ambient_k * ambient_color;

	float diffuse_k = 1.0;
	vec3 diffuse_color = texture(textureMap, texcoord).rgb;
	// vec3 diffuse_color = vec3(0.35, 0.3, 0.15);
	vec3 diffuse = diffuse_k * diffuse_color * max(dot(normal, light), 0.0);

	float specular_k = 0.3;
	vec3 specular_color = vec3(1.0, 1.0, 1.0);
	vec3 object_color = texture(specularMap, texcoord).rgb;
	float shininess = 28.0;
	vec3 reflector = 2.0 * dot(light, normal) * normal - light;
	if (dot(light, normal) <= 0.0) reflector = vec3(0.0, 0.0, 0.0);
	vec3 specular = specular_k * specular_color * object_color * pow(max(dot(reflector, view), 0.0), shininess);

	outColor = vec4(ambient + diffuse + specular, 1.0);

}