R"(
#version 450 core

layout (location=0) uniform mat4 matrix;
layout (location=1) in vec3 Vertex;
layout (location=2) in vec2 tex_coord;


out VS_OUT
{
  vec2 tex_coord;
} vs_out;

void main(void)
{
  vs_out.tex_coord=tex_coord;
  gl_Position = matrix*vec4(Vertex,1.0);
}
)"
