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

// point light color
uniform vec3 color;
uniform int objNum;
uniform vec3 pointLightPos;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor;

vec3 calcPointLight(vec3 ambient, vec3 diffuse2, vec3 specular, vec3 viewDir2) {

	// get required parameter for diffuse formula
	vec3 normal = normalize(normCoord);
	vec3 lightDir = normalize(lightPos - fragPos);

	// apply formula and multiply with light color
	float diff = max(dot(normal, lightDir), 0.0f);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir2), 0.1), specPhong);

	/* Point Light */

	// attenuation
	float distance = length(lightPos - fragPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse2 *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse2 + specular);

}

void main() {

	vec3 result;

	// get required parameter for diffuse formula
	vec3 normal = normalize(normCoord);

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

	result = (ambientCol + diffuse + specColor);

	result += calcPointLight(ambientCol, diffuse, specColor, viewDir);

	//FragColor = vec4(result, 1.0) * texture(tex0, texCoord);

	if(objNum == 1) {
		FragColor = vec4(result, 1.0) * texture(tex0, texCoord);
	} else {
		FragColor = vec4(1, 1, 1, 1.0);
	}

}