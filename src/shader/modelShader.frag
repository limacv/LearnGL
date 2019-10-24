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
in vec3 normal_;
in vec3 fragPos_;
in vec2 texCood;

void main()
{
    fragColor = texture(object.A_D, texCood);
}