#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 speed;
out vec3 ourColor;
out vec3 vel;
void main()
{
   gl_Position =  projection * view * model * vec4(aPos, 1.0);
   vec4 buf = model * vec4(aPos, 1.0f);
   ourColor = (vec3 (buf.x, buf.y, buf.z) - 40.0f)/20;
   vel = speed/10.0f;
}
