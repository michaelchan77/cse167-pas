#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 rotate;

void main()
{
    gl_Position = rotate * vec4(aPos, 1.0f);
}
