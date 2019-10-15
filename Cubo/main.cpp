#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <GL/glew.h> /* include GLEW and new version of GL on Windows */
#include <GLFW/glfw3.h> /* GLFW helper library */
#include <GLM/glm.hpp>
#include <GLM/vec2.hpp>
#include <GLM/vec3.hpp>
#include <GLM/vec4.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <Soil.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Includes.h"
#include "Mesh.h"
#include "Material.h"
#include "Obj3D.h"

#define _USE_MATH_DEFINES
#include <cmath>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale, int idObjeto);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//camera
float camSpeed = 0.01f;
float dirSpeed = 0.01f;

float pitchAngle = -30.f;
float yawAngle = -90.f;
glm::vec3 camDirection = glm::vec3(
	glm::normalize(glm::vec3(
		cos(glm::radians(pitchAngle)) * cos(glm::radians(yawAngle)),
		sin(glm::radians(pitchAngle)),
		cos(glm::radians(pitchAngle)) * sin(glm::radians(yawAngle))
	))
);

glm::vec3 camPosition(0.0f, 1.0f, 3.0f);
glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
glm::vec3 camFront = glm::normalize(glm::cross(camDirection, worldUp));

glm::mat4 ViewMatrix = glm::lookAt(camPosition, camPosition + camDirection, worldUp);

int selecionado = 0;

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const char* vertexShaderSource =
		"#version 410 core\n"
		"layout(location = 0) in vec3 aPos;"
		"layout (location = 2) in vec2 aTexCoord;"
		"layout (location = 1) in vec3 aNormal;"
		"out vec3 ourPos;"
		"out vec2 TexCoord;"
		"out vec3 ourNormal;"
		"uniform mat4 ModelMatrix;"
		"uniform mat4 ViewMatrix;"
		"uniform mat4 ProjectionMatrix;"
		"void main() {"
		"   ourPos = vec4(ModelMatrix * vec4(aPos, 1.f)).xyz;"
		"   TexCoord = aTexCoord;"
		"   ourNormal = mat3(ModelMatrix) * aNormal;"
		"   gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.f);"
		"}";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint sucess;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &sucess);
	if (!sucess) {
		cout << "Error Vertex";
	}

	const char* fragmentShaderSource =
		"#version 410 core\n"
		"in vec3 ourPos;"
		"in vec2 TexCoord;"
		"in vec3 ourNormal;"
		"uniform bool selecionado;"
		"out vec4 FragColor;"
		"uniform sampler2D texture1;"
		"void main()	{"
		"if (selecionado) {"
		"   FragColor = texture(texture1, TexCoord) * vec4(0.5,0.2,0.5,1.0);"
		"}else{"
		"   FragColor = texture(texture1, TexCoord) * vec4(1.0,1.0,1.0,1.0);"
		"}"
		"}";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
	GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
	GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

	glUseProgram(shaderProgram);

	glm::vec3 position(0.f);
	glm::vec3 rotation(0.f);
	glm::vec3 scale(1.f);

	glm::mat4 ModelMatrix(1.f);
	ModelMatrix = glm::translate(ModelMatrix, position);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
	ModelMatrix = glm::scale(ModelMatrix, scale);

	glm::vec3 position2(0.5f, 0.2f, 0.0f);
	glm::vec3 rotation2(0.0f);
	glm::vec3 scale2(1.f);

	glm::mat4 ModelMatrix2(1.f);
	ModelMatrix2 = glm::translate(ModelMatrix2, position2);
	ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(rotation2.x), glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(rotation2.y), glm::vec3(0.f, 1.f, 0.f));
	ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(rotation2.z), glm::vec3(0.f, 0.f, 1.f));
	ModelMatrix2 = glm::scale(ModelMatrix2, scale2);

	float fov = 90.f;
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	glm::mat4 ProjectionMatrix(1.f);

	ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	//Mesh* readMesh = readOBJ("teste2.obj");
	//loadVertices(readMesh);

	Obj3D obj;
	Mesh* readMesh = obj.processObj("teste2.obj");
	vector<Material*> materials = obj.getMat();

	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		processInput(window, position, rotation, scale, 1);
		processInput(window, position2, rotation2, scale2, 2);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		GLuint texture1;
		for (Group* g : readMesh->groups) {
			for (Material* m : materials) {
				texture1 = m->texture;
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
			glUniform1i((glGetUniformLocation(shaderProgram, "selecionado")), selecionado == 1);

			glBindVertexArray(g->vao);
			glDrawArrays(GL_TRIANGLES, 0, g->faces.size() * 3);

			glUniform1i((glGetUniformLocation(shaderProgram, "selecionado")), selecionado == 2);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix2));

			glDrawArrays(GL_TRIANGLES, 0, g->faces.size() * 3);

		}

		glfwPollEvents();

		glm::vec3 camFrontCalc;
		camFrontCalc.x = cos(glm::radians(pitchAngle)) * cos(glm::radians(yawAngle));
		camFrontCalc.y = sin(glm::radians(pitchAngle));
		camFrontCalc.z = cos(glm::radians(pitchAngle)) * sin(glm::radians(yawAngle));
		camDirection = glm::normalize(camFrontCalc);

		ViewMatrix = glm::lookAt(camPosition, camPosition + camDirection, worldUp);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));

		ModelMatrix = glm::mat4(1.f);
		ModelMatrix = glm::translate(ModelMatrix, position);
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
		ModelMatrix = glm::scale(ModelMatrix, scale);

		ModelMatrix2 = glm::mat4(1.f);
		ModelMatrix2 = glm::translate(ModelMatrix2, position2);
		ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(rotation2.x), glm::vec3(1.f, 0.f, 0.f));
		ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(rotation2.y), glm::vec3(0.f, 1.f, 0.f));
		ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(rotation2.z), glm::vec3(0.f, 0.f, 1.f));
		ModelMatrix2 = glm::scale(ModelMatrix2, scale2);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));

		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		ProjectionMatrix = glm::mat4(1.f);
		ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale, int idObjeto)
{
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		selecionado = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		selecionado = 2;
	}

	if (selecionado == idObjeto)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			position.y += 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			position.y += 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			position.x -= 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			position.y -= 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			position.x += 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			rotation.y -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			rotation.y += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			rotation.x -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
			rotation.x += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			scale += 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			scale -= 0.001f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camPosition += camSpeed * camDirection;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camPosition -= camSpeed * camDirection;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		yawAngle -= dirSpeed;
		camFront = cross(camDirection, worldUp);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		yawAngle += dirSpeed;
		camFront = cross(camDirection, worldUp);
	}
	
}