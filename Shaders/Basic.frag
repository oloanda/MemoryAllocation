#version 330

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

};

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;


uniform bool isColor;
uniform bool noLights;
uniform bool hasSpecularMap;
uniform bool noAttenuation;

uniform sampler2D uDiffuseMap;
uniform sampler2D uNormalMap;
uniform sampler2D uSpecularMap;

uniform Material uMaterial;
uniform Light uLight;
uniform vec3 uViewPos;

void main()
{
    vec3 result;
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(uLight.position - FragPos);
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float distance = length(uLight.position - FragPos);
    float attenuation = 1.0 / (uLight.quadratic * (distance * distance) + uLight.linear * distance + uLight.constant);

    if(isColor)
    {
        if(!noLights)
        {
            vec3 ambient = uLight.ambient * uMaterial.diffuse;

            float diff = max(dot(lightDir,normal), 0.0);
            vec3 diffuse = uLight.diffuse * diff * uMaterial.diffuse;

            float spec = pow(max(dot(normal,reflectDir), 0.0), uMaterial.shininess);
            vec3 specular = uLight.specular * spec * uMaterial.diffuse;

            //if(noAttenuation)
            {
                //ambient *= attenuation;
                //diffuse *= attenuation;
                //specular *= attenuation;
            }

            result += ambient + diffuse + specular;

        }
        else
        {
            result = uMaterial.diffuse;
        }
    }
    else
    {
        if(!noLights)
        {
            
            vec3 specular;
            vec3 ambient = uLight.ambient * texture(uDiffuseMap, TexCoord).rgb;

            float diff = max(dot(lightDir,normal), 0.0);
            vec3 diffuse = uLight.diffuse * diff * texture(uDiffuseMap, TexCoord).rgb;

            float spec = pow(max(dot(viewDir,reflectDir), 0.0), uMaterial.shininess);
            if(hasSpecularMap)
                specular = uLight.specular * spec * texture(uSpecularMap, TexCoord).rgb;
            else
                specular = uLight.specular * spec * uMaterial.specular;


            //if(noAttenuation)
            {
                //ambient *= attenuation;
                //diffuse *= attenuation;
                //specular *= attenuation;
            }

            result += ambient + diffuse + specular;
        }
        else
        {
            result =  texture(uDiffuseMap, TexCoord).rgb;
        }

    }

    FragColor = vec4(result, 1.0);
}