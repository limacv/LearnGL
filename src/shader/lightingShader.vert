#version 330 core
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexPos;

out vec3 normal;
out vec3 fragPos;
out vec2 texCood;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    fragPos = vec3(model * vec4(aPosition, 1.0f));
    gl_Position = projection * view * vec4(fragPos, 1);
    normal = mat3(transpose(inverse(model))) * aNormal;
    texCood = aTexPos;
}
