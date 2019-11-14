#include "../headers/System.h"


System::System(Runnable* runnable) {
    this->runnable = runnable;
}


System::~System() {}


int System::GLFWInit() {

    if (!glfwInit()) {
        std::cout << "ERROR: Could not start GLFW3" << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = glfwCreateWindow(runnable->width(), runnable->height(), runnable->screenTitle().c_str(), NULL, NULL);


    if (!window) {
        std::cout << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    if (runnable->init(window) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void System::run() {
    while (runnable->keepRunning(window)) {
        runnable->run(window);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    this->finish();
}

void System::finish() {
    runnable->finish();
    glfwTerminate();
}
