#version 330 core
uniform vec3 bulbColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(bulbColor,1.0f); // 将向量的四个分量全部设置为1.0
}