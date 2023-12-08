#version 330 core


struct GSout{
    vec3 FragPos;
    vec3 norm;
    vec2 TexCoords;
};
in GSout gs_out;

//textures
uniform sampler2D AmbientTexture;
uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D SpecularHighlightTexture;
uniform sampler2D AlphaTexture;
uniform sampler2D BumpTexture;
//material parameters
uniform vec3 AmbientColor;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform float SpecularExponent;
uniform float OpticalDensity;
uniform float Dissolve;
uniform int Illumination;
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

vec3 DirLightContr(DirectionalLight dl){
    vec3 ambient = 0.05 * AmbientColor * texture(DiffuseTexture, gs_out.TexCoords).rgb;
    // diffuse
    vec3 lightDir = normalize(dl.dir);
    vec3 normal = normalize(gs_out.norm);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * DiffuseColor * texture(DiffuseTexture, gs_out.TexCoords).rgb;
    // specular
    vec3 viewDir = normalize(viewPos - gs_out.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        float sp = (SpecularExponent*2);
        if(mod(sp, 2) == 1) sp++;
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(dot(normal, halfwayDir), sp);
    }
    else
    {
        float sp = (SpecularExponent);
        if(mod(sp, 2) == 1) sp++;
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(dot(viewDir, reflectDir), sp);
    }
    vec3 specular = SpecularColor * spec * texture(SpecularTexture, gs_out.TexCoords).rgb;
    return vec3(ambient + diffuse + specular) * dl.color;
}
vec3 PointLightContr(PointLight pl){
    vec3 ambient = 0.05 * AmbientColor * texture(DiffuseTexture, gs_out.TexCoords).rgb;
    // diffuse
    vec3 lightDir = normalize((pl.pos)-gs_out.FragPos);
    vec3 normal = normalize(gs_out.norm);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * DiffuseColor * texture(DiffuseTexture, gs_out.TexCoords).rgb;
    // specular
    vec3 viewDir = normalize(viewPos - gs_out.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        float sp = (SpecularExponent*2);
        if(mod(sp, 2) == 1) sp++;
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(dot(normal, halfwayDir), sp);
    }
    else
    {
        float sp = (SpecularExponent);
        if(mod(sp, 2) == 1) sp++;
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(dot(viewDir, reflectDir), sp);
    }
    vec3 specular = SpecularColor * spec * texture(SpecularTexture, gs_out.TexCoords).rgb;
    float d = length(pl.pos - gs_out.FragPos);
    float attenuation = 1.0 / (pl.constant + pl.linear * d + 
  			     pl.quadratic * (d * d));
    return (vec3(ambient + diffuse + specular) * attenuation) * pl.color;
}
vec3 SpotLightContr(SpotLight sl){
    vec3 lightDir = normalize((sl.pos)-gs_out.FragPos);
    float theta = dot(lightDir, normalize(-sl.dir)); 
    float epsilon   = sl.cutOff - sl.outerCutOff;
    float intensity = clamp((theta - sl.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = 0.05 * AmbientColor * texture(DiffuseTexture, gs_out.TexCoords).rgb;
    // diffuse
    vec3 normal = normalize(gs_out.norm);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * DiffuseColor * texture(DiffuseTexture, gs_out.TexCoords).rgb;
    // specular
    vec3 viewDir = normalize(viewPos - gs_out.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        float sp = (SpecularExponent*2);
        if(mod(sp, 2) == 1) sp++;
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(dot(normal, halfwayDir), sp);
    }
    else
    {
        float sp = (SpecularExponent);
        if(mod(sp, 2) == 1) sp++;
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(dot(viewDir, reflectDir), sp);
    }
    vec3 specular = SpecularColor * spec * texture(SpecularTexture, gs_out.TexCoords).rgb;
    float d = length(sl.pos - gs_out.FragPos);
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
    if(texture(DiffuseTexture, gs_out.TexCoords).a < 0.5f) discard;
    FragColor = vec4(dirLightContr + spotLightContr + pointLightContr, 1);
}