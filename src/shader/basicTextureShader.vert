#version 330 core
layout(location=0) in vec3 Position;
layout(location=1) in vec4 inColor;
layout(location=2) in vec2 texPos;

out vec4 vertexColor;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(Position, 1.0f);
    vertexColor = inColor;
    texCoord = vec2(texPos.x, (1-texPos.y));
}
