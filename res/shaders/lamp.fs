#version 410 core
uniform vec3 lampColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(lampColor, 1.0);
}