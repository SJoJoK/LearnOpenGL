#version 330 core
#define RENDER 0
#define NORMAL 1
#define AO 2
#define ALBEDO 3
#define SPECULAR 4
#define ROUGHNESS 5
#define MODEL 0
const float PI = 3.1415926;
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_normal1;
    sampler2D texture_normal2;
    sampler2D texture_AO1;
    sampler2D texture_AO2;
    sampler2D texture_roughness1;
    sampler2D texture_roughness2;
};
struct DirLight {
    vec3 direction;
    vec3 lightColor;
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
in VS_OUT {
    vec3 FragPos;
    vec4 FragPosLightSpace;
    vec2 TexCoord;
    mat3 TBN;
} fs_in;
out vec4 FragColor;
uniform sampler2D shadowMap;
uniform DirLight dirLight_PBR;
uniform PointLight pointLight_PBR;
uniform Material material;
uniform bool shadowOn;
uniform bool gammaOn;
uniform bool HDROn;
uniform vec3 viewPos;
uniform int renderMode;
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float k);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float ShadowCalculation(DirLight dirLight, PointLight pointLight, vec3 normal, vec4 fragPosLightSpace);
void main()
{
    vec3 normal = texture(material.texture_normal1, fs_in.TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
    normal = normalize(fs_in.TBN * normal);
    vec3 viewDir = normalize(viewPos-fs_in.FragPos);

    float shadow = ShadowCalculation(dirLight_PBR, pointLight_PBR, normal, fs_in.FragPosLightSpace);

    vec3 ao = texture(material.texture_AO1,fs_in.TexCoord).rrr;
    vec3 albedo = texture(material.texture_diffuse1,fs_in.TexCoord).rgb;
    vec3 metallic = texture(material.texture_specular1,fs_in.TexCoord).rrr;
    float roughness = texture(material.texture_roughness1,fs_in.TexCoord).r;

    vec3 lightDir = -normalize(dirLight_PBR.direction);
    vec3 halfVec = normalize(viewDir + lightDir);

    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(halfVec, viewDir), 0.0), F0);

    vec3 Lo = vec3(0.0);

    float NDF = DistributionGGX(normal, halfVec, roughness);       
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001; 
    vec3 specular     = nominator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, lightDir), 0.0);
    
    vec3 radiance = dirLight_PBR.lightColor;
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 result   = ambient + Lo;
    if(shadowOn)
    {
        result = ambient + (1-shadow)*Lo;
    }
    if(HDROn)
    {
        result = result / (result + vec3(1.0));
    }
    if(gammaOn)
    {
        float gamma = 2.2;
        result.rgb = pow(result.rgb, vec3(1.0/gamma));
    }
    if(renderMode==RENDER)
        FragColor = vec4(result,1.f);
    else if(renderMode==NORMAL)
        FragColor = vec4((normal+1)/2,1.f);
    else if(renderMode==AO)
        FragColor = vec4(texture(material.texture_AO1, fs_in.TexCoord).rrr,1);
    else if(renderMode==ALBEDO)
        FragColor = vec4(texture(material.texture_diffuse1, fs_in.TexCoord).rgb,1);
    else if(renderMode==SPECULAR)
        FragColor = vec4(texture(material.texture_specular1, fs_in.TexCoord).rgb,1);
    else if(renderMode==ROUGHNESS)
        FragColor = vec4(texture(material.texture_roughness1, fs_in.TexCoord).rrr,1);
}
float ShadowCalculation(DirLight dirLight, PointLight pointLight, vec3 normal, vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, -dirLight.direction)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}