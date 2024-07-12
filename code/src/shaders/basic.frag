R"(
#version 450 core

uniform sampler2D texture_image;

in VS_OUT
{
  vec2 tex_coord;
} fs_out;

out vec4 frag_color;

void main(void)
{
  frag_color = texture(texture_image,fs_out.tex_coord);
}
)"
