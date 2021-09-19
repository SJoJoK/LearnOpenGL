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
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 
out vec4 FragColor;
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
uniform Light light;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform Material material;
uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos-FragPos);

    // ��һ�׶Σ��������
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // �ڶ��׶Σ����Դ
    result += CalcPointLight(pointLight, norm, FragPos, viewDir);
    // �����׶Σ��۹�
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    

    FragColor = vec4(result, 1.0);
}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse_m, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse_m, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular_m, TexCoord));
    return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥��
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse_m, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse_m, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular_m, TexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}