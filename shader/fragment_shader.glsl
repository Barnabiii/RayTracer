#version 460 core

out vec4 FragColor;
  
in vec3 ourColor;
in vec2 textureCoord;

uniform sampler2D container_texture;

void main()
{
    FragColor = texture(container_texture, textureCoord);
}