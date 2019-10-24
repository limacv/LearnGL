#version 330 core

in vec4 vertexColor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D image;
uniform sampler2D image1;

uniform float a;

void main()
{
    fragColor = mix(texture(image, texCoord), texture(image1, texCoord), a);
}