#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <unordered_map>

class Input
{
public:
	static bool GetKey(unsigned int key_code);
	static bool GetKeyDown(unsigned int key_code);
	static bool GetMouseButtonDown(unsigned int button);
	static bool GetMouseButton(unsigned int button);
	static GLFWwindow* GetWindow();

private:
	GLFWwindow * window;
	std::unordered_map<unsigned int, bool> keys_pressed;
	std::unordered_map<unsigned int, bool> keys_pressed_last;
	std::unordered_map<unsigned int, bool> mouse_buttons;
	std::unordered_map<unsigned int, bool> mouse_buttons_last;

	static Input* Instance;

	static void Update();
	Input(GLFWwindow* window);
	~Input();

	friend class Relic;
};

#endif