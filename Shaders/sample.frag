#version 330 core // version

uniform sampler2D tex0;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float ambientStr;
uniform vec3 ambientColor;

// for direction light
uniform vec3 dirLight;

// for point light
uniform float constant;
uniform float linear;
uniform float quadratic;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor;

void main() {
	// FragColor = vec4(0.7f, 0f, 0f, 1f);
	vec3 result;

	// get required parameter for diffuse formula
	vec3 normal = normalize(normCoord);
	// vec3 lightDir = normalize(lightPos - fragPos);
	vec3 lightDir = normalize(-dirLight);

	// apply formula and multiply with light color
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;

	// get the ambient light
	vec3 ambientCol = ambientColor * ambientStr;

	// get view direction and reflection vector
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	// get specular light
	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
	vec3 specColor = spec * specStr * lightColor;

	result = ambientCol + diffuse + specColor;

	/* Point Light */

	// get required parameter for diffuse formula
	normal = normalize(normCoord);
	lightDir = normalize(lightPos - fragPos);

	// apply formula and multiply with light color
	diff = max(dot(normal, lightDir), 0.0f);
	//diffuse = diff * lightColor;

	// get the ambient light
	ambientCol = ambientColor * ambientStr;

	// get view direction and reflection vector
	viewDir = normalize(cameraPos - fragPos);
	reflectDir = reflect(-lightDir, normal);

	// get specular light
	spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
	//specColor = spec * specStr * lightColor;

	// attenuation
	float distance = length(lightPos - fragPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	// combine
	vec3 ambient = ambientCol * vec3(texture(tex0, texCoord));
	vec3 diffuse2 = diffuse * diff * vec3(texture(tex0, texCoord));
	vec3 specular = specColor * spec  * vec3(texture(tex0, texCoord));

	ambient *= attenuation;
	diffuse2 *= attenuation;
	specular *= attenuation;

	result += ambient + diffuse2 + specular; 

	// apply the lighting to the texture on the fragment
	//FragColor = vec4(specColor + diffuse + ambientCol, 1.0f) * texture(tex0, texCoord);
	FragColor = vec4(result, 1.0) * texture(tex0, texCoord);

	//FragColor = texture(tex0, texCoord);
}