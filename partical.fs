#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec3 vel;
void main()
{
    FragColor = vec4(1.0f, 1.0f, 1.0f, 0.0f) - vec4(vel, 1.0f);
}
