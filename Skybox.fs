#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform float light;
uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords) * light;
}