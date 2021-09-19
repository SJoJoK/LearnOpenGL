#version 330 core
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    sampler2D texture_normal1;
    sampler2D texture_normal2;
    sampler2D texture_normal3;
    sampler2D texture_height1;
    sampler2D texture_height2;
    sampler2D texture_height3;
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
in VS_OUT {
    vec3 FragPos;
    vec4 FragPosLightSpace;
    vec2 TexCoord;
    mat3 TBN;
} fs_in;
uniform sampler2D shadowMap;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform Material material;
uniform bool gammaOn;
uniform vec3 viewPos;
uniform int renderMode;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);
void main()
{
    vec3 normal = texture(material.texture_normal1, fs_in.TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
    normal = normalize(fs_in.TBN * normal);
    vec3 viewDir = normalize(viewPos-fs_in.FragPos);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    // ��һ�׶Σ��������
    vec3 result = CalcDirLight(dirLight, normal, viewDir, shadow);
    // �ڶ��׶Σ����Դ
    //result += CalcPointLight(pointLight, normal, fs_in.FragPos, viewDir);
    // �����׶Σ��۹�
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    if(gammaOn)
    {
        float gamma = 2.2;
        result.rgb = pow(result.rgb, vec3(1.0/gamma));
    }
    if(renderMode==0)
        FragColor = vec4(result,1.f);
    else if(renderMode==1)
        FragColor = vec4((normal+1)/2,1.f);
}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoord));
    return (ambient + (1-shadow) * (diffuse + specular));
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // ˥��
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // �任��[0,1]�ķ�Χ
    projCoords = projCoords * 0.5 + 0.5;
    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}