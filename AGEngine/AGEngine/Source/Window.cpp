#include <PCH.h>
#include <Window.h>


Window::Window(){
 memset(m_KeyState, 0, 256 * sizeof(char));
 memset(&m_MousePosition, 0, sizeof(POINT));
 m_MouseWheel = 0.0f;
	}

void Window::OnWindowInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)

{
	switch (msg)
	{
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	{
		int button = 0;
		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = VK_LBUTTON; }
		if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = VK_RBUTTON; }
		if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = VK_MBUTTON; }
		if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2; }
		if (!AnyMouseDown() && ::GetCapture() == NULL)
			::SetCapture(hwnd);
		m_KeyState[button] = 1;
		return;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	{
		int button = 0;
		if (msg == WM_LBUTTONUP) { button = VK_LBUTTON; }
		if (msg == WM_RBUTTONUP) { button = VK_RBUTTON; }
		if (msg == WM_MBUTTONUP) { button = VK_MBUTTON; }
		if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2; }
		m_KeyState[button] = 0;
		if (!AnyMouseDown() && ::GetCapture() == hwnd)
			::ReleaseCapture();
		return;
	}
	case WM_MOUSEWHEEL:
		//io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		m_MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return;
	case WM_MOUSEHWHEEL:
		//io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return;
	case WM_MOUSEMOVE:
		GetCursorPos(&m_MousePosition);
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256)
			m_KeyState[wParam] = 1;
		return;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256)
			m_KeyState[wParam] = 0;
		return;
		//case WM_SETCURSOR:
		//	if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
		//		return;
		//	return;
	case WM_DEVICECHANGE:
		return;
	};
	return;
}

void Window::UpdateMousePos(HWND hwnd)
{
	if (HWND active_window = ::GetForegroundWindow())
		if (active_window == hwnd || ::IsChild(active_window, hwnd))
			if (::GetCursorPos(&m_MousePosition) && ::ScreenToClient(hwnd, &m_MousePosition))
				return;
}

bool Window::AnyMouseDown()
{
	for (int i = 0; i < 7; ++i)
	{
		if (m_KeyState[i])
			return true;
	}
	return false;
}

void Window::Update()
{
	memcpy(m_previouseKeyState, m_KeyState, 256 * sizeof(char));
	memcpy(&m_MousePreviousPosition, &m_MousePosition, sizeof(POINT));
	m_PrevMouseWheel = m_MouseWheel;
}

void Window::CallUpdateCallbacks()
{
	for (auto t : mouseUpdateCallbacks)
	{
		t(GetMouseMove());
	}

	for (auto t : keyUpdateCallbacks)
	{
		t();
	}
}

bool Window::IsPressed(uint16_t _keyScanCode)
{

	// keyboard
	if (_keyScanCode >= 256)
		return false;

	if (m_KeyState[_keyScanCode])
		return true;

	return false;
}

bool Window::IsTriggered(uint16_t _keyScanCode)
{

	// keyboard
	if (_keyScanCode >= 256)
		return false;

	if (m_KeyState[_keyScanCode] && !m_previouseKeyState[_keyScanCode])
		return true;

	return false;
}

bool Window::IsReleased(uint16_t _keyScanCode)
{

		// keyboard
		if (_keyScanCode >= 256)
			return false;

		if (!m_KeyState[_keyScanCode] && m_previouseKeyState[_keyScanCode])
			return true;

		return false;
	}

glm::vec2 Window::GetMousePos()
{
	return glm::vec2(m_MousePosition.x, m_MousePosition.y);
}
glm::vec2 Window::GetMouseMove()
{
	return glm::vec2(m_MousePosition.x - m_MousePreviousPosition.x, m_MousePosition.y - m_MousePreviousPosition.y) * 0.1f;
}