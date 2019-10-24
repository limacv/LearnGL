#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 normal[];
in vec3 fragPos[];
in vec2 texCood[];
uniform float time;

out vec3 normal_;
out vec3 fragPos_;
out vec2 texCood_;

vec4 explode(vec4 position, vec3 normal);
vec3 GetNormal();

void main()
{
    vec3 n = GetNormal();

    for (int i = 0; i < 3; ++i)
    {
        gl_Position = explode(gl_in[i].gl_Position, n);
        normal_ = normal[i];
        fragPos_ = fragPos[i];
        texCood_ = texCood[i];
        EmitVertex();
    }
    EndPrimitive();
}

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = -normal * sin(time) * magnitude; 
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}