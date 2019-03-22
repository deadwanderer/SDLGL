#version 330 core
out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

#define NUM_LIGHTS 4
uniform sampler2D floorTexture;
uniform vec3 lightPositions[NUM_LIGHTS];
uniform vec3 lightColors[NUM_LIGHTS];
uniform vec3 viewPos;
uniform bool blinn;
uniform bool gamma;

vec3 CalcLight(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
	// diffuse
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;
	
	// specular
	vec3 viewDir = normalize(viewPos - fragPos);	
	float spec = 0.0;
	if(blinn) 
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	} 
	else 
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
	}
	vec3 specular = spec * lightColor;
	float max_distance = 1.5;
	float distance = length(lightPos - fragPos);
	float attenuation = 1.0 / (gamma ? distance * distance : distance);
	
	diffuse *= attenuation;
	specular *= attenuation;
	return (diffuse + specular);
	
}

void main()
{
	vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
	vec3 result = vec3(0.0);
	for (int i = 0; i < NUM_LIGHTS; i++)
		result += CalcLight(normalize(fs_in.Normal), fs_in.FragPos, lightPositions[i], lightColors[i]);
	color *= result;
	if (gamma)
		color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color, 1.0);
}