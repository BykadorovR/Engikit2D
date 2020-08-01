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
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 color;

void main()
{
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * light_color;

	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light_color;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * light_color;  

    color = texture2D(u_TextureUnit, vec2(v_TextureCoordinates.x, v_TextureCoordinates.y));
	color *= vec4(color_mask, 1.0);
	color += vec4(color_addition, 1.0);

	color = vec4(ambient + diffuse + specular, 1.0) * color;
}