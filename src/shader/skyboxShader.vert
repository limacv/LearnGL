#version 330 core
layout(location=0) in vec3 aPosition;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCood;

void main()
{
    gl_Position = (projection * mat4(mat3(view)) * vec4(aPosition, 1.0f)).xyww;//early z test
    texCood = aPosition;
}
