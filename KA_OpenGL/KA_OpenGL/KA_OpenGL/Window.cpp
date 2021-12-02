#include "Window.h"

Window::Window()
{
	// Default Screen Resolution
	width = 1920 / RESOLUTION;
	height = 1080 / RESOLUTION;

	// Clear array
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int Window::Intialise()
{
	if (!glfwInit())
	{
		printf("GLFW Initialisation failed!");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Creates Actual Window. Must be after the Hints
	mainWindow = glfwCreateWindow(width, height, "Kilian OpenGL Engine", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get buffer size information and store it in bufferWidth ans bufferHeight
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	// holds all of OpenGL
	glfwMakeContextCurrent(mainWindow);
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);


	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Set the GLFW window ref to this (i guess)
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	// Everytime a key will be pressed, play "handleKeys" callback
	glfwSetKeyCallback(mainWindow, handleKeys);

	// Everytime the mouse cursor moves, play "handleMouse" callback
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getxChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getyChange()
{
	GLfloat theChange =yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		// Close the window
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// If the key entered is in ASCII range
	if (key >= 0 && key < 1024)
	{		
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("Pressed: %d\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("Released: %d\n", key);
		}
	}
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mousedFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mousedFirstMoved = false;
	}

	// Mouse Delta Position Calcul
	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
