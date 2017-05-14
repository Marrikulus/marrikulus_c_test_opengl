#version 400

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D textureContainer;
uniform float inalpha;
uniform sampler2D textureSmile;

void main() {

	float alpha = 0.0f;
	if(inalpha > 0)
	{
		alpha = inalpha;
	}

	fragColor = mix(
		texture(textureContainer , texCoord),
		texture( textureSmile, vec2(texCoord.x, texCoord.y)),
		alpha
	);
}
