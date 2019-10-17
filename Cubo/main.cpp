#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <vector>

#include "Headers/Includes.h"
#include "Headers/Mesh.h"
#include "Headers/Material.h"
#include "Headers/Obj3D.h"
#include "Headers/Shader.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale, int idObjeto);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//camera
float camSpeed = 0.01f;
float dirSpeed = 0.01f;

bool firtMouse = true;

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

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Computacao Grafica - GA", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	//mouse
	glfwSetCursorPosCallback(window, mouse_callback);

	glewExperimental = GL_TRUE;
	glewInit();

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//read shaders
	GLuint shaderProgram = LoadShader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
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

	Obj3D obj;
	Mesh* readMesh = obj.processObj("teste2.obj");
	vector<Material*> materials = obj.getMat();

	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		processInput(window, position, rotation, scale, 1);
		processInput(window, position2, rotation2, scale2, 2);

		int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		GLuint texture;
		for (Group* g : readMesh->groups) {
			for (Material* m : materials) {
				texture = m->texture;
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0);
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	
	if (pressed == 1) {
		cout << "mouse pressionado";
	}
	else {

	}
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

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		camPosition += camSpeed * camDirection;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camPosition -= camSpeed * camDirection;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		yawAngle -= dirSpeed;
		camFront = cross(camDirection, worldUp);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		yawAngle += dirSpeed;
		camFront = cross(camDirection, worldUp);
	}

}