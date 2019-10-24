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
uniform vec3 camPos;
uniform samplerCube sky;

in vec3 normal;
in vec3 fragPos;
in vec2 texCood;

// --------------------------------------------------------
void main()
{
    vec3 inlight = normalize(fragPos - camPos);
    
    float cosTheta = -dot(inlight, normalize(normal));
    vec3 reflight;
    if(cosTheta > 0.3)
        // refract action
        reflight = refract(inlight, normalize(normal), 0.8);
    else if(cosTheta > 0)
        // reflect action
        reflight = reflect(inlight, normalize(normal));
    else
        discard;

    fragColor = vec4(texture(sky, reflight));
}