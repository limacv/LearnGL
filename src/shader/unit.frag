#version 330 core

out vec4 fragColor;

in vec2 texCood;
uniform sampler2D tex;
uniform vec4 color;

void main()
{
    fragColor = texture(tex, texCood);
    // fragColor = color;
}