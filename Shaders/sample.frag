#version 330 core // version

uniform sampler2D tex0;
uniform sampler2D norm_tex;
uniform sampler2D tex1;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float ambientStr;
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

in mat3 TBN;

out vec4 FragColor;

void main() {
	// FragColor = vec4(0.7f, 0f, 0f, 1f);

	// Current pixel
	// 0.6 opacity required, use mix to blend the two textures which will be the combination of the two textures
	vec4 pixelColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), 0.6);

	if(pixelColor.a < 0.1) {
		discard;
	}

	// get required parameter for diffuse formula
	vec3 normal = texture(norm_tex, texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(TBN * normal);

	vec3 lightDir = normalize(lightPos - fragPos);

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

	// apply the lighting to the texture on the fragment
	FragColor = vec4(specColor + diffuse + ambientCol, 1.0f) * pixelColor;

	//FragColor = texture(tex0, texCoord);
}