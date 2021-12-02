#pragma once

#include <stdio.h>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "CommonValues.h"

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);

	int Intialise();

	// Classic Getters
	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	bool* getsKeys() { return keys;}
	GLfloat getxChange();
	GLfloat getyChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	bool keys[1024]; // 1024 cover the ascII character, so the inputs

	// Mouse DeltaPosition
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange = 0.0f;
	GLfloat yChange = 0.0f;
	bool mousedFirstMoved;

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode); // static function for a Callback
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

