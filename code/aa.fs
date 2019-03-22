#version 330 core
out vec4 FragColor;

void main() {
    float ambientStrength = 0.1;
    vec3 lightColor = vec3(0.0, 0.3, 0.2);
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 objectColor = vec3(0.4, 0.3, 0.6);
    vec3 result = objectColor * ambient;
	FragColor = vec4(result, 1.0);
}