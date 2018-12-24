#version 450

layout(binding = 0) uniform sampler2D textureMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D specularMap;

in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 texcoord;
in vec3 view;
in vec3 light;

out vec4 outColor;

void main() {
	

	vec3 norm_color = texture(normalMap, texcoord).rgb;
	float norm_r = norm_color.x * 2.0 - 1.0;
	float norm_g = norm_color.y * 2.0 - 1.0;
	float norm_b = norm_color.z * (-2.0) + 1.0;
	vec3 new_norm = normalize(norm_r * tangent + norm_g * bitangent + norm_b * normal);

	vec3 reflector = 2.0 * dot(light, new_norm) * new_norm - light;

	float ambient_k = 0.1;
	vec3 ambient_color = vec3(0.7, 0.7, 0.7);
	vec3 ambient = ambient_k * ambient_color;

	float diffuse_k = 1.0;
	vec3 diffuse_color = texture(textureMap, texcoord).rgb;
	vec3 diffuse = diffuse_k * diffuse_color * dot(new_norm, light);

	float specular_k = 0.3;
	vec3 specular_color = vec3(1.0, 1.0, 1.0);
	float shininess = 1.0;
	vec3 specular = specular_k * specular_color * pow(dot(view, reflector), shininess);

	outColor = vec4(ambient + diffuse + specular, 1.0);

}