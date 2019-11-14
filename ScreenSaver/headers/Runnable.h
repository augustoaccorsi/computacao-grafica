#ifndef RUNNABLE_H
#define RUNNABLE_H

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>

class Runnable {

public:
	Runnable() {}
	virtual ~Runnable() {};

	/*
	 * Este método é chamado antes de todos para inicializar o que precisa antes da execução
	 * Events/Callback
	 * Shaders/Programs
	 *
	 */
	virtual int init(GLFWwindow* window) = 0;

	/*
	 * Código que deve ser rodado dentro do loop while (render)
	 *
	 */
	virtual void run(GLFWwindow* window) = 0;

	/*
	 * Condição se deve continuar rodando
	 *
	 */
	virtual bool keepRunning(GLFWwindow* window) = 0;

	/*
	 * Método chamado ao final da execução
	 *
	 */
	virtual void finish() = 0;

	/*
	 *  Retorna o 'width' da janela
	 *
	 */
	virtual int width() = 0;

	/*
	 * Retorna o 'height' da janela
	 *
	 */
	virtual int height() = 0;

	/*
	 * Retorna o título da janela
	 */
	virtual std::string screenTitle() = 0;

};
#endif
