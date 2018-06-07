#include "Input.h"
#include "Util.h"

Input::~Input()
{
}

Input* Input::Instance;

//This function should be called every frame for this to work properly
void Input::Update()
{
	for(unsigned int key = 32; key < GLFW_KEY_LAST; key++)
	{
		try {
			Instance->keys_pressed_last.at(key) = Instance->keys_pressed.at(key);
		}
		catch (std::out_of_range exception)
		{
			Instance->keys_pressed_last.insert(std::pair<unsigned, bool>(key, Instance->keys_pressed.at(key)));
		}

		try
		{
			Instance->keys_pressed.at(key) = glfwGetKey(Instance->window, key);
		}
		catch (std::out_of_range exception)
		{
			Instance->keys_pressed.insert(std::pair<unsigned, bool>(key, glfwGetKey(Instance->window, key)));
		}
	}

	for(unsigned int button = 0; button < GLFW_MOUSE_BUTTON_LAST; button++)
	{
		try
		{
			Instance->mouse_buttons_last.at(button) = Instance->mouse_buttons.at(button);
		}
		catch (std::out_of_range exception)
		{
			Instance->mouse_buttons_last.insert(std::pair<unsigned, bool>(button, Instance->mouse_buttons.at(button)));
		}

		try
		{
			Instance->mouse_buttons.at(button) = glfwGetMouseButton(Instance->window, button);
		}
		catch (std::out_of_range exception)
		{
			Instance->mouse_buttons.insert(std::pair<unsigned, bool>(button, Instance->mouse_buttons.at(button)));
		}
	}
}

Input::Input(GLFWwindow* window)
{
	Instance = this;
	this->window = window;
	for(unsigned int key = 32; key < GLFW_KEY_LAST; key++)
	{
		keys_pressed_last.insert(std::pair<unsigned, bool>(key, false));
		keys_pressed.insert(std::pair<unsigned, bool>(key, false));
	}

	for(unsigned int button = 0; button < GLFW_MOUSE_BUTTON_LAST; button++)
	{
		mouse_buttons_last.insert(std::pair<unsigned, bool>(button, false));
		mouse_buttons.insert(std::pair<unsigned, bool>(button, false));
	}
}

bool Input::GetKey(unsigned int key_code)
{
	return Instance->keys_pressed.at(key_code);
}

bool Input::GetKeyDown(unsigned key_code)
{
	if (Instance->keys_pressed_last.empty()) return Instance->keys_pressed.at(key_code);
	 
	return Instance->keys_pressed.at(key_code) && !Instance->keys_pressed_last.at(key_code);
}

bool Input::GetMouseButtonDown(unsigned button)
{
	if (Instance->mouse_buttons_last.empty()) return Instance->mouse_buttons.at(button);

	return Instance->mouse_buttons.at(button) && !Instance->mouse_buttons_last.at(button);
}

bool Input::GetMouseButton(unsigned button)
{
	return Instance->mouse_buttons.at(button);
}

GLFWwindow* Input::GetWindow()
{
	return Instance->window;
}
