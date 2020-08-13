#version 430

precision mediump float;
//bind sampler2D to specific texture unit = GL_TEXTURE0. Can be changed via code
//glActiveTexture(GL_TEXTURE0 + 1);
//glBindTexture(GL_TEXTURE_2D, id);
//to use simultaneously several textures in shader we should pass textures to different GL_TEXTUREx
in vec2 v_TextureCoordinates;
in vec3 FragPos;
in vec3 v_Normal;

uniform vec3 viewPos;

out vec4 color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
  
struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;

void main()
{
	vec3 ambient = light.ambient * texture(material.diffuse, v_TextureCoordinates).rgb;

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));  

	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(light.position - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, v_TextureCoordinates).rgb;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, v_TextureCoordinates).rgb;  

    
	color = vec4(attenuation * (ambient + diffuse + specular), 1.0);
}