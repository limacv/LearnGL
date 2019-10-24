#version 330 core
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexPos;
layout(location=3) in mat4 instanceMat;

out vec2 texCood;

uniform mat4 modelPlanet;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    vec3 fragPos = vec3(instanceMat * modelPlanet * vec4(aPosition, 1.0f));
    gl_Position = projection * view * vec4(fragPos, 1);
    texCood = aTexPos;
}
