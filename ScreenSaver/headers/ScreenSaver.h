#ifndef TRIANGULOS_H
#define TRIANGULOS_H
#include "Runnable.h"
#include "Shader.h"
#include <ostream>
#include <iostream>


class ScreenSaver : public Runnable {
private:
    GLint WIDTH, HEIGHT;
    char* SCREEN_TITLE;

    //dados que o init() cria e o run() utiliza
    Shader* shader;
    GLuint VBO, VAO, colorVBO;

    //callback and eventHandlers
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void processInput(GLFWwindow* window);


public:
    ScreenSaver(int width, int height, char* screenTitle);
    ~ScreenSaver();

    int init(GLFWwindow* window) override;
    void run(GLFWwindow* window) override;
    bool keepRunning(GLFWwindow* window) override;
    void finish() override;
    int width() override;
    int height() override;
	std::string screenTitle() override;
};

#endif
