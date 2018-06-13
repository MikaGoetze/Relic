#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <GLFW/glfw3.h>

class Window
{
public:
	//Initialise  window with given parameters
	bool Initialise(int width, int height, std::string title, bool windowed, int monitor = 0);

	//Initialise window on given monitor, in fullscreen, at default resolution
	bool Initialise( std::string title, int monitor);

	void SetWindowTitle(std::string title);
	void SetWindowWidth(int width);
	void SetWindowHeight(int height);

	Window();
	~Window();

	const int& WindowWidth() const
	{
		return windowWidth;
	}

	const int& WindowHeight() const
	{
		return windowHeight;
	}

	const std::string& WindowTitle() const
	{
		return windowTitle;
	}

	GLFWwindow* InternalWindow() const
	{
		return window;
	}

	bool ShouldClose() const
	{
		return glfwWindowShouldClose(window);
	}

private:
	int windowWidth;
	int windowHeight;
	std::string windowTitle;
	GLFWwindow* window = NULL;
	bool initialised;

	friend class Relic;
};

#endif
