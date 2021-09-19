#version 330 core
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D diffuse_m;
    sampler2D specular_m;
};
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
out vec4 FragColor;
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
uniform Light light;
uniform Material material;
uniform vec3 viewPos;
void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse_m, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse_m, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular_m, TexCoord));
    
    vec3 result = ambient+diffuse+specular;

    FragColor = vec4(result, 1.0);
}