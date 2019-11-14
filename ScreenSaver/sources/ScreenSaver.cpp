#include "../headers/ScreenSaver.h"

float matrix[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.25f, 0.25f, 0.0f, 1.0f
};

ScreenSaver::ScreenSaver(int width, int height, char* screenTitle) {
    this->WIDTH = width;
    this->HEIGHT = height;
    this->SCREEN_TITLE = screenTitle;
}

ScreenSaver::~ScreenSaver() { }

int ScreenSaver::init(GLFWwindow* window) {
    shader = new Shader("./shaders/core/vertex.vert", "./shaders/core/fragment.frag");

    /* tell GL to only draw onto a pixel if the shape is closer to the viewer */
    glEnable(GL_DEPTH_TEST); /* enable depth-testing */
    glDepthFunc(GL_LESS); /*depth-testing interprets a smaller value as "closer"*/


    GLfloat points[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

    return EXIT_SUCCESS;
}

float speed = 1.0f;
float lastPosition = 0.0f;
void ScreenSaver::run(GLFWwindow* window) {
    processInput(window);

	static double previousSeconds = glfwGetTime();
	double currentSeconds = glfwGetTime();
	double elapsedSeconds = currentSeconds - previousSeconds;
	if (elapsedSeconds > 0) {
		previousSeconds = currentSeconds;
		if (fabs(lastPosition) > 1.0f) {
			speed = -speed;
		}
		matrix[12] = elapsedSeconds * speed + lastPosition;
		lastPosition = matrix[12];


        // N = normal
        // . = Produto Escalar - v1.x * v2.x + v1.y * v2.y
        // 2*N*(N.R)-R
        //  
        // N = (0, -1)
        //
        //
	}

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
	shader->setMatrix4fv("matrix", matrix);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

bool ScreenSaver::keepRunning(GLFWwindow* window) {
    return !glfwWindowShouldClose(window);
}

void ScreenSaver::finish() {
    glDeleteBuffers(1, &colorVBO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
}

int ScreenSaver::width() {
    return this->WIDTH;
}

int ScreenSaver::height() {
    return this->HEIGHT;
}

std::string ScreenSaver::screenTitle() {
    return this->SCREEN_TITLE;
}


void ScreenSaver::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void ScreenSaver::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
