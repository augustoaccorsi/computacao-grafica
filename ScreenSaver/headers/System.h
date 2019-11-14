#ifndef SYSTEM_H
#define SYSTEM_H

// External Libs
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <cstdio>
#include <ostream>
#include <iostream>

#include "Runnable.h"

class System {

private:
    Runnable* runnable;

    void finish();

public:
    GLFWwindow* window;

    System(Runnable* runnable);
    ~System();

    int GLFWInit();
    void run();

};


#endif
