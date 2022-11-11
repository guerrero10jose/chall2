#version 330 core // version

uniform sampler2D tex0;

in vec2 texCoord;

out vec4 FragColor;

void main() {
	// FragColor = vec4(0.7f, 0f, 0f, 1f);

	FragColor = texture(tex0, texCoord);
}