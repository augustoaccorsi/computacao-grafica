#version 410 core

layout(location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 1) in vec3 aNormal;
out vec3 ourPos;
out vec2 TexCoord;
out vec3 ourNormal;
out vec3 eye_position, eye_normal;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
void main() {
   eye_position = vec3(view * model * vec4(vp, 1.0));
   eye_normal = vec3(view * model * vec4(vn, 0.0));
   ourPos = vec4(ModelMatrix * vec4(aPos, 1.f)).xyz;
   TexCoord = aTexCoord;
   ourNormal = mat3(ModelMatrix) * aNormal;
   gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.f);
}