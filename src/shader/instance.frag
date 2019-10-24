// inplement pointlight, spotlight or paralight
#version 330 core
struct Material {
    sampler2D A_D;
    sampler2D S;
    // float shininess;
};

// output
out vec4 fragColor;

// fragment propreties
uniform Material object;
in vec2 texCood;

void main()
{
    fragColor = texture(object.A_D, texCood);
}