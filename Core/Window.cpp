#include "Window.h"
#include <iostream>
#include "Util.h"


bool Window::Initialise(int width, int height, std::string title, bool windowed, int monitor)
{
	if(window != NULL)
	{
		Util::Log("[Relic][Window] Warning : Attempted to initialise a window while another window is active. Discarding...");
		return false;
	}

	windowHeight = height;
	windowWidth = width;
	windowTitle = title;

	window = NULL;
	if(windowed)
	{
		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	}
	else
	{
		int monitor_count;
		GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
		if(monitor_count == 0)
		{
			Util::Log("[Relic][Window] Error : Failed to initialise window in fullscreen mode.");
			return false;
		}
		if(monitor >= monitor_count)
		{
			Util::Log("[Relic][Window] Error : Can not create window on monitor " + std::to_string(monitor) + ".");
			return false;
		}

		window = glfwCreateWindow(width, height, title.c_str(), monitors[monitor], NULL);
	}

	glfwMakeContextCurrent(window);

	initialised = true;
	return true;
}

bool Window::Initialise(std::string title, int monitor)
{
	if(window != NULL)
	{
		Util::Log("[Relic][Window] Warning : Attempted to initialise a window while another window is active. Discarding...");
		return false;
	}

	int monitor_count;
	GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
	if(monitor_count == 0)
	{
		Util::Log("[Relic][Window] Error : Failed to initialise window in fullscreen mode.");
		return false;
	}
	if (monitor >= monitor_count)
	{
		Util::Log("[Relic][Window] Error : Can not create window on monitor " + std::to_string(monitor) + ".");
		return false;
	}


	const GLFWvidmode * vidMode = glfwGetVideoMode(monitors[monitor]);

	windowHeight = vidMode->height;
	windowWidth = vidMode->width;
	windowTitle = title;

	window = glfwCreateWindow(vidMode->width, vidMode->height, title.c_str(), monitors[monitor], NULL);
	glfwMakeContextCurrent(window);

	initialised = true;
	return true;
}

void Window::SetWindowTitle(std::string title)
{
	windowTitle = title;

	glfwSetWindowTitle(window, title.c_str());
}

void Window::SetWindowWidth(int width)
{
	windowWidth = width;
}

void Window::SetWindowHeight(int height)
{
	windowHeight = height;
}

Window::Window()
{
}


Window::~Window()
{
}
