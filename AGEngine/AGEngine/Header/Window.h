#pragma once
#include <Singleton.h>
class Window:public Singleton <Window>{
	Window();
	friend class Singleton <Window>;
public:

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
};