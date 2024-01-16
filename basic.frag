#version 330 core
out vec4 FragColor;

in vec3 chNormal;  
in vec3 chFragPos;  
in vec2 chUV;

uniform vec3 viewPos; 
uniform bool transparent;

// Direkciono svetlo
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

// Tackasto svetlo
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 3
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform bool turnedOn[NR_POINT_LIGHTS];

// Reflektor svetlo
struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;  
};
uniform SpotLight spotLight;

uniform sampler2D uDiffMap1;
uniform sampler2D uSpecMap1;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, bool inHouse);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,bool inHouse);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,bool inHouse);
void main()
{
    // properties
    vec3 norm = normalize(chNormal);
    vec3 viewDir = normalize(viewPos - chFragPos);
    bool inHouse = false;

    if(chFragPos.x > -0.09521 && chFragPos.x < 0.064 && chFragPos.z > -0.10577 && chFragPos.z < -0.082567 && chFragPos.y < 0.5){
        inHouse = true;
    }
    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir, inHouse);
    // phase 2: Point lights

     if(turnedOn[1] && pointLights[0].position.y < chFragPos.y){
        result += CalcPointLight(pointLights[1], norm, chFragPos, viewDir, inHouse);  
    }
    if(turnedOn[0] && pointLights[0].position.y > chFragPos.y){
        result += CalcPointLight(pointLights[0], norm, chFragPos, viewDir, inHouse);  
    }
   
    result += CalcPointLight(pointLights[2], norm, chFragPos, viewDir, inHouse);  

    // phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, chFragPos, viewDir, inHouse);    
    
    if(transparent){
    FragColor = vec4(result, 0.5);
    }else{
    FragColor = vec4(result, 1.0);
    }
    
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, bool inHouse)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(uDiffMap1, chUV));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(uDiffMap1, chUV));
    vec3 specular = texture(uSpecMap1, chUV).r * spec * light.specular;

    if(inHouse){
    return ambient;
    }
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool inHouse)
{
    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * light.ambient;
        
    // diffuse 
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(uDiffMap1, chUV));

    // specular
    float specularStrength = texture(uSpecMap1, chUV).r;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light.specular;

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    if(inHouse){
    return ambient;
    }

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool inHouse){
    
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
    
    // ambinet
    vec3 ambient = light.ambient * vec3(texture(uDiffMap1, chUV));
    ambient *= attenuation;

      if(inHouse){
    return ambient;
    }

    // diffuse 
    vec3 diffuse = light.diffuse * diff * vec3(texture(uDiffMap1, chUV));
    diffuse *= attenuation;

    // specular
    vec3 specular = light.specular * spec * vec3(texture(uSpecMap1, chUV));
    specular *= attenuation;

    float theta = acos(dot(-lightDir, normalize(light.direction)));
    float epsilon = radians(light.cutOff);

    vec3 result = ambient;
    if (theta < epsilon) {
      result += diffuse;
      result += specular;
    }

 

    return result;
}