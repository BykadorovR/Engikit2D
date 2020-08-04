#version 430

precision mediump float;
//bind sampler2D to specific texture unit = GL_TEXTURE0. Can be changed via code
//glActiveTexture(GL_TEXTURE0 + 1);
//glBindTexture(GL_TEXTURE_2D, id);
//to use simultaneously several textures in shader we should pass textures to different GL_TEXTUREx
layout(binding=0) uniform sampler2D u_TextureUnit;
in vec2 v_TextureCoordinates;
in vec3 FragPos;
in vec3 v_Normal;

uniform vec3 color_mask;
uniform vec3 color_addition;
uniform vec3 light_color;
uniform vec3 viewPos;

out vec4 color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{
	vec3 ambient = light.ambient * material.ambient;

	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(light.position - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * material.diffuse;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * material.specular;  

    color = texture2D(u_TextureUnit, vec2(v_TextureCoordinates.x, v_TextureCoordinates.y));
	color *= vec4(color_mask, 1.0);
	color += vec4(color_addition, 1.0);

	color = vec4(ambient + diffuse + specular, 1.0);
}