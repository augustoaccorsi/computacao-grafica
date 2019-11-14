#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

using namespace std;

int main() {
	GLFWwindow* window = NULL;
	const GLubyte* renderer;
	const GLubyte* version;
	GLuint vao, vao2;
	GLuint vbo, vbo2;
	
	typedef struct Vertex {
		GLfloat coords[3];
	} VecXYZ;

	VecXYZ points[3];
	//v1 top
	points[0].coords[0] = 0.0f; //x
	points[0].coords[1] = 0.5f; //y
	points[0].coords[2] = 0.0f;
	//v2 right
	points[1].coords[0] = 0.25f; //x
	points[1].coords[1] = -0.25f; //y
	points[1].coords[2] = 0.0f;
	//v3 left
	points[2].coords[0] = -0.25f; //x
	points[2].coords[1] = -0.25f; //y
	points[2].coords[2] = 0.0f;
	
	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	
	float matrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.1f, 0.1f, 0.0f, 1.0f
	};
	
	const char* vertex_shader =
		"#version 410\n"
		"layout(location=0) in vec3 vp;"
		"layout(location=1) in vec3 vc;"
		"uniform mat4 matrix;"
		"out vec3 color;"
		"void main () {"
		"   color = vc;"
		"	gl_Position = matrix * vec4 (vp, 1.0);"
		"}";

	const char* fragment_shader =
		"#version 410\n"
		"in vec3 color;"
		"out vec4 frag_color;"
		"void main () {"
		"	frag_color = vec4 (color, 1.0);"
		"}";
	GLuint vs, fs, fs2;
	GLuint shader_programme, shader_programme2;

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(
		640, 480, "Screen Saver", NULL, NULL
	);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	renderer = glGetString(GL_RENDERER);
	version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo); 
										
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint colorsVBO;
	glGenBuffers(1, &colorsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1); 

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	int matrixLocation = glGetUniformLocation(shader_programme, "matrix");

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

	float speedX = 0.5f;
	float speedY = 0.5f;
	float nextPositionX = 0.0f;
	float nextPositionY = 0.0f;
	float lastPositionX = 0.0f;
	float lastPositionY = 0.0f;
	glUseProgram(shader_programme);
	while (!glfwWindowShouldClose(window)) {

		static double previousSeconds = glfwGetTime();
		double currentSeconds = glfwGetTime();
		double elapsedSeconds = currentSeconds - previousSeconds;
		if (elapsedSeconds > 0) {
			previousSeconds = currentSeconds;

			if (fabs(points[1].coords[0] + matrix[12]) >= 1.0f) {
				float normalX = -1.0f;
				float rX = lastPositionX - 1.0f;
				float pEscalarX = rX * normalX;
				speedX = 2 * normalX * pEscalarX - rX;
			}

			if (fabs(points[2].coords[1] + matrix[12]) >= 1.0f) {
				float normalX = 1.0f;
				float rX = lastPositionX - (-1.0f);
				float pEscalarX = rX * normalX;
				speedX = 2 * normalX * pEscalarX - rX;
			}

			if (fabs(points[0].coords[1] + matrix[13]) >= 1.0f) {
				float normalY = -1.0f;
				float rY = lastPositionY - 1.0f;
				float pEscalarX = rY * normalY;
				speedY = 2 * normalY * pEscalarX - rY;
			}

			if (fabs(points[1].coords[1] + matrix[13]) >= 1.0f || fabs(points[2].coords[1] + matrix[13]) >= 1.0f) {
				float normalY = 1.0f;
				float rY = lastPositionY - (-1.0f);
				float pEscalarX = rY * normalY;
				speedY = 2 * normalY * pEscalarX - rY;
			}

			matrix[12] = elapsedSeconds * speedX + lastPositionX;
			lastPositionX = matrix[12];

			matrix[13] = elapsedSeconds * speedY + lastPositionY;
			lastPositionY = matrix[13];
		}

		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, matrix);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwPollEvents();

		glfwSwapBuffers(window);
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
	}

	glfwTerminate();
	return 0;
}