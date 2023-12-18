#version 330 core

in vec3 FragPos;
in vec3 norm;
in vec2 TexCoords;

//textures
uniform sampler2D AmbientTexture;
uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D AlphaTexture;
//material parameters
uniform vec3 AmbientColor;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform float SpecularExponent;
uniform bool blinn;




//Lights
uniform vec3 viewPos;
uniform vec3 DirLightDir;
struct DirectionalLight {
    vec3 dir;
    vec3 color;
};
uniform DirectionalLight DirectionalLights[5];
uniform int DirectionalLightAmt = 0;

struct PointLight{
    vec3 pos;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};
uniform PointLight PointLights[5];
uniform int PointLightAmt = 0;
struct SpotLight{
    vec3 pos;
    vec3 dir;
    vec3 color;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight SpotLights[5];
uniform int SpotLightAmt = 0;



out vec4 FragColor;

vec3 getTexColorTriplanar(sampler2D targetTex) {
    vec3 texTop = texture(targetTex, FragPos.xz).rgb;
    vec3 texSide = texture(targetTex, FragPos.xy).rgb;
    vec3 texFront = texture(targetTex, FragPos.yz).rgb;
    vec3 tex = texTop * norm.y + texSide * norm.z * texFront * norm.x;
    return tex;
}

vec3 DirLightContr(DirectionalLight dl){
    vec3 ambient = 0.05 * AmbientColor * getTexColorTriplanar(AmbientTexture);
    // diffuse
    vec3 lightDir = normalize(dl.dir);
    vec3 normal = normalize(norm);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * DiffuseColor * getTexColorTriplanar(DiffuseTexture);
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(norm, halfwayDir), 0.0), SpecularExponent*2);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), SpecularExponent);
    }
    vec3 specular = SpecularColor * spec * getTexColorTriplanar(SpecularTexture);
    return vec3(ambient + diffuse + specular) * dl.color;
}
vec3 PointLightContr(PointLight pl){
    vec3 ambient = 0.05 * AmbientColor * getTexColorTriplanar(AmbientTexture);
    // diffuse
    vec3 lightDir = normalize((pl.pos)-FragPos);
    vec3 normal = normalize(norm);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * DiffuseColor * getTexColorTriplanar(DiffuseTexture);
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), SpecularExponent*2);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), SpecularExponent);
    }
    vec3 specular = SpecularColor * spec * getTexColorTriplanar(SpecularTexture);
    float d = length(pl.pos - FragPos);
    float attenuation = 1.0 / (pl.constant + pl.linear * d + 
  			     pl.quadratic * (d * d));
    return (vec3(ambient + diffuse + specular) * attenuation) * pl.color;
}
vec3 SpotLightContr(SpotLight sl){
    vec3 lightDir = normalize((sl.pos)-FragPos);
    float theta = dot(lightDir, normalize(-sl.dir)); 
    float epsilon   = sl.cutOff - sl.outerCutOff;
    float intensity = clamp((theta - sl.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = 0.05 * AmbientColor * getTexColorTriplanar(AmbientTexture);
    // diffuse
    vec3 normal = normalize(norm);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * DiffuseColor * getTexColorTriplanar(DiffuseTexture);
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), SpecularExponent*2);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), SpecularExponent);
    }
    vec3 specular = SpecularColor * spec * getTexColorTriplanar(SpecularTexture);
    float d = length(sl.pos - FragPos);
    float attenuation = 1.0 / (sl.constant + sl.linear * d + 
  			     sl.quadratic * (d * d));
    return (vec3(ambient + (diffuse + specular) * intensity) * attenuation) * sl.color;
}

void main()
{   
    vec3 pointLightContr = vec3(0.f);
    for(int i = 0; i < PointLightAmt; i++){
        pointLightContr += PointLightContr(PointLights[i]);
    }
    vec3 spotLightContr = vec3(0.f);
    for(int i = 0; i < SpotLightAmt; i++){
        spotLightContr += SpotLightContr(SpotLights[i]);
    }
    vec3 dirLightContr = vec3(0.f);
    for(int i = 0; i < DirectionalLightAmt; i++){
        dirLightContr += DirLightContr(DirectionalLights[i]);
    }
    // ambient
    FragColor = vec4(dirLightContr + pointLightContr + spotLightContr, texture(AlphaTexture, TexCoords).r);
}