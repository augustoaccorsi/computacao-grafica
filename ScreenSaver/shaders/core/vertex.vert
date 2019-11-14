#version 460 core
layout (location = 0) in vec3 vertex;   // the position variable has attribute position 0
layout (location = 1) in vec3 inColor; // the color variable has attribute position 1

uniform mat4 matrix;
  
out vec3 color; // output a color to the fragment shader

void main() {
    gl_Position = matrix * vec4(vertex, 1.0);
    color = inColor; // set ourColor to the input color we got from the vertex data
}    