#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 all_trans;

void main()
{
    gl_Position = all_trans * vec4(position, 1.0f);
    // gl_Position = vec4(position, 1.0f);
    
}