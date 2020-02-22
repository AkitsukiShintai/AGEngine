#pragma once
class Graphics;
class Window:public Singleton <Window>{
	Window();
	friend class Singleton <Window>;
	friend class Graphics;
#ifdef GLFW
	GLFWwindow* window;
	static void MouseInput(GLFWwindow* window, int button, int action, int mods);
	static void KeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseMove(GLFWwindow* window, double xpos, double ypos);
	static void ScrollInput(GLFWwindow* window, double xoffset, double yoffset);
	static void WindowClose(GLFWwindow* window);
	const static int keySize = 300;
	 char keyState[keySize];
	 char prevKeyState[keySize];
	 glm::vec2 mousePosition;
	 glm::vec2 prevMousePosition;

	 glm::vec2 mouseWheel;
	 glm::vec2 prevMouseWheel;

#else
	//window handle
	HWND winWindowHandle;
	HINSTANCE winAppliocationInstance;

#endif // GLFW

public:
	bool close;
	//window information
	std::string windowName;
	size_t width;
	size_t height;
	std::string applicationName;
	void Init();
	void Clear();
#ifdef GLFW

	using InputCallback = void(*)(Window * window);

	std::vector<InputCallback> inputCallback;

	
	void Update();
	bool IsPressed(int key);

	bool IsTriggered(int key);

	bool IsReleased(int key);

	glm::vec2 GetMousePos();

	glm::vec2 GetMouseMove();
#else
	std::vector<MouseUpdateCallback> mouseUpdateCallbacks;
	std::vector<KeyUpdateCallback> keyUpdateCallbacks;
	char m_KeyState[256];
	char m_previouseKeyState[256];
	POINT m_MousePosition;
	float m_MouseWheel;
	POINT m_MousePreviousPosition;
	float m_PrevMouseWheel;
	void OnWindowInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void UpdateMousePos(HWND hwnd);
	bool AnyMouseDown();

	void Update();

	void CallUpdateCallbacks();

	bool IsPressed(uint16_t _keyScanCode);

	bool IsTriggered(uint16_t _keyScanCode);

	bool IsReleased(uint16_t _keyScanCode);

	glm::vec2 GetMousePos();

	glm::vec2 GetMouseMove();

	void SetWindowHandler(HWND _winWindowHandle);
#endif
	
};