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

vector<Material*> materials;

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

GLuint loadTexture(const char* filename) {
	// Enabling texture processing
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

	int texWidth, texHeight, nrChannels;
	unsigned char* data;

	GLuint texture;

	// Loading image with filename from parameter
	data = stbi_load(filename, &texWidth, &texHeight, &nrChannels, 0);

	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data) {
		// The image I chose has no alpha channel, so using GL_RGB to process it
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	return texture;
}

void readMTL(const string filename) {
	Material* m = nullptr;

	ifstream arq(filename);

	while (!arq.eof()) {
		string line;
		getline(arq, line);
		stringstream sline;
		sline << line;
		string temp;
		sline >> temp;

		if (temp == "newmtl") {
			if (m != nullptr) {
				materials.push_back(m);
			}
			string mtlName;
			sline >> mtlName;
			m = new Material(mtlName);
		}
		else if (temp == "Ka") {
			float r, g, b;
			sline >> r >> g >> b;
			m->ka = new glm::vec3(r, g, b);
		}
		else if (temp == "Kd") {
			float r, g, b;
			sline >> r >> g >> b;
			m->kd = new glm::vec3(r, g, b);
		}
		else if (temp == "Ks") {
			float r, g, b;
			sline >> r >> g >> b;
			m->ks = new glm::vec3(r, g, b);
		}
		else if (temp == "Ns") {
			float ns;
			sline >> ns;
			m->ns = ns;
		}
		else if (temp == "map_Kd") {
			string textureFile;
			sline >> textureFile;
			m->texture = loadTexture(textureFile.c_str());
		}
	}
	materials.push_back(m);
}

// Read OBJ file
Mesh* readOBJ(const string filename) {
	auto mesh = new Mesh;
	Group* g = nullptr;

	ifstream arq(filename);

	while (!arq.eof()) {
		string line;
		getline(arq, line);
		stringstream sline;
		sline << line;
		string temp;
		sline >> temp;
		if (temp == "mtllib") {
			string mtlFile;
			sline >> mtlFile;
			readMTL(mtlFile);
		}
		else if (temp == "v") {
			float x, y, z;
			sline >> x >> y >> z;
			auto* v = new glm::vec3(x, y, z);
			mesh->vertex.push_back(v);
		}
		else if (temp == "vt") {
			float x, y;
			sline >> x >> y;
			auto* v = new glm::vec2(x, y);
			mesh->mappings.push_back(v);
		}
		else if (temp == "vn") {
			float x, y, z;
			sline >> x >> y >> z;
			auto* v = new glm::vec3(x, y, z);
			mesh->normals.push_back(v);
		}
		else if (temp == "g") {
			if (g != nullptr) {
				mesh->groups.push_back(g);
			}
			string inName;
			sline >> inName;
			g = new Group(inName, "default");
		}
		else if (temp == "usemtl") {
			if (g == nullptr) {
				g = new Group("default", "default");
			}
			string inMaterial;
			sline >> inMaterial;
			g->material = inMaterial;
		}
		else if (temp == "f") {
			if (g == nullptr) {
				g = new Group("default", "default");
			}
			auto* f = new Face();
			while (!sline.eof()) {
				string token;
				sline >> token;
				if (token.empty()) {
					continue;
				}
				stringstream stoken;
				stoken << token;
				string aux[3];
				int countParam = -1;
				do {
					countParam = countParam + 1;
					getline(stoken, aux[countParam], '/');
				} while (!stoken.eof());
				for (int i = 0; i < 3; i = i + 1) {
					switch (i) {
					case 0:
						if (aux[i].empty()) {
							f->verts.push_back(-1);
						}
						else {
							f->verts.push_back(stoi(aux[i]) - 1);
						}
						break;
					case 1:
						if (aux[i].empty()) {
							f->texts.push_back(-1);
						}
						else {
							f->texts.push_back(stoi(aux[i]) - 1);
						}
						break;
					case 2:
						if (aux[i].empty()) {
							f->norms.push_back(-1);
						}
						else {
							f->norms.push_back(stoi(aux[i]) - 1);
						}
						break;
					default:
						break;
					}
				}
			}
			g->faces.push_back(f);
		}
	}
	mesh->groups.push_back(g);
	return mesh;
}

void loadVertices(Mesh* mesh) {

	for (Group* g : mesh->groups) {
		vector<float> vs;
		vector<float> vn;
		vector<float> vt;

		for (Face* f : g->faces) {
			for (int i = 0; i < f->verts.size(); i = i + 1) {
				int vi = f->verts[i];
				glm::vec3* v = mesh->vertex[vi];
				vs.push_back(v->x);
				vs.push_back(v->y);
				vs.push_back(v->z);
			}

			for (int i = 0; i < f->verts.size(); i = i + 1) {
				int vi = f->norms[i];
				glm::vec3* v = mesh->normals[vi];
				vn.push_back(v->x);
				vn.push_back(v->y);
				vn.push_back(v->z);
			}

			for (int i = 0; i < f->verts.size(); i = i + 1) {
				int vi = f->texts[i];
				glm::vec2* v = mesh->mappings[vi];
				vt.push_back(v->x);
				vt.push_back(v->y);
			}
		}

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// VBO for vertices
		GLuint vboVerts;
		glGenBuffers(1, &vboVerts);
		glBindBuffer(GL_ARRAY_BUFFER, vboVerts);
		glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(float), vs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		// VBO for normals
		GLuint vboNorms;
		glGenBuffers(1, &vboNorms);
		glBindBuffer(GL_ARRAY_BUFFER, vboNorms);
		glBufferData(GL_ARRAY_BUFFER, vn.size() * sizeof(float), vn.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		// VBO for texture mappings
		GLuint vboTexts;
		glGenBuffers(1, &vboTexts);
		glBindBuffer(GL_ARRAY_BUFFER, vboTexts);
		glBufferData(GL_ARRAY_BUFFER, vt.size() * sizeof(float), vt.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		// Store VAO for group
		g->vao = vao;
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
		"layout (location = 1) in vec2 aTexCoord;"
		"layout (location = 2) in vec3 aNormal;"
		"out vec3 ourPos;"
		"out vec2 TexCoord;"
		"out vec3 ourNormal;"
		"uniform mat4 ModelMatrix;"
		"uniform mat4 ViewMatrix;"
		"uniform mat4 ProjectionMatrix;"
		"void main() {"
		"   ourPos = vec4(ModelMatrix * vec4(aPos, 1.f)).xyz;"
		"   TexCoord = vec2(aTexCoord.x, aTexCoord.y * -1.0f);"
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
		"   FragColor = texture(texture1, TexCoord);"
		"if (selecionado) {"
		"   FragColor = vec4(0.5,0.2,0.5,1.0);"
		"}else{"
		"   FragColor = vec4(1.0,1.0,1.0,1.0);"
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

	glm::mat4 model
		(1.0f, 0.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 0.0f, 
		0.0f, 0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 1.0f);

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

	Mesh* readMesh = readOBJ("teste2.obj");
	loadVertices(readMesh);
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