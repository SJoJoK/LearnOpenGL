#version 330 core
uniform vec3 bulbColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(bulbColor,1.0f); // ���������ĸ�����ȫ������Ϊ1.0
}