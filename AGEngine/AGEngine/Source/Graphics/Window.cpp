#include <PCH.h>
#include <Graphics/Window.h>

void Window::Init()
{
#ifdef GLFW
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, applicationName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(window, KeyInput);
	glfwSetCursorPosCallback(window, MouseMove);
	glfwSetMouseButtonCallback(window, MouseInput);
	glfwSetScrollCallback(window, ScrollInput);
	glfwSetWindowCloseCallback(window, WindowClose);
	glfwSetWindowAspectRatio(window, 16, 9);
	//glfwMaximizeWindow(window);
#else
	HINSTANCE g_hAppInstance = (HINSTANCE)GetModuleHandle(NULL);

	WNDCLASSEX wndClassDesc = { sizeof(WNDCLASSEX) };
	wndClassDesc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wndClassDesc.hbrBackground = NULL;
	wndClassDesc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndClassDesc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClassDesc.hInstance = g_hAppInstance;
	wndClassDesc.lpfnWndProc = WndProc;
	wndClassDesc.lpszClassName = applicationName.c_str();

	const ATOM hWndClass = RegisterClassEx(&wndClassDesc);
	assert(hWndClass);

	const DWORD style = WS_OVERLAPPEDWINDOW;
	const DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	RECT rect = { 0, 0, width, height };
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	CreateWindowEx(
		exStyle, applicationName.c_str(), applicationName.c_str(), style,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, g_hAppInstance, NULL);
	ShowWindow(winWindowHandle, SW_SHOW);
	SetForegroundWindow(winWindowHandle);
	SetFocus(winWindowHandle);
#endif


}

void Window::Clear()
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
	glfwDestroyWindow(window);
	glfwTerminate();
}


#ifdef GLFW
Window::Window( ):close(false){
	memset(keyState, 0, keySize * sizeof(char));
	memset(prevKeyState, 0, keySize * sizeof(char));
	glfwInit();
}

void Window::MouseInput(GLFWwindow* window, int button, int action, int mods)
{
	Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->keyState[button] = action;
}

void Window::KeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (key < keySize)
	{
		myWindow->keyState[key] = action;
	}
	
}

void Window::MouseMove(GLFWwindow* window, double xpos, double ypos)
{
	Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->mousePosition.x = xpos;
	myWindow->mousePosition.y = ypos;
}

void Window::ScrollInput(GLFWwindow* window, double xoffset, double yoffset)
{
	Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->mouseWheel.x = xoffset;
	myWindow->mouseWheel.y = xoffset;
}

void Window::WindowClose(GLFWwindow* window)
{
	Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->close = true;
}

void Window::Update() {
	glfwPollEvents();

	//update  event
	for (auto t : inputCallback)
	{
		t(this);
	}
	
	memcpy(prevKeyState, keyState, 256 * sizeof(char));
	prevMouseWheel = mouseWheel;
	prevMousePosition = mousePosition;
}
bool Window::IsPressed(int key) {
	
	if (keyState[key])
	{
		return true;
	}
	return false;
}

bool Window::IsTriggered(int key) {
	if (keyState[key] && !prevKeyState[key])
	{
		return true;
	}
	return false;
}

bool Window::IsReleased(int key) {
	if (!keyState[key] && prevKeyState[key])
	{
		return true;
	}
	return false;
}

glm::vec2 Window::GetMousePos() {
	return mousePosition;
}

glm::vec2 Window::GetMouseMove() {
	return mousePosition - prevMousePosition;
}



#else

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window::GetInstance()->OnWindowInput(hWnd, msg, wParam, lParam);
	Graphics* g = Graphics::GetInstance();
	switch (msg)
	{
	case WM_CREATE:
		// This is intentionally assigned here because we are now inside CreateWindow, before it returns.
		Window::GetInstance()->SetWindowHandler(hWnd);
		return 0;

	case WM_DESTROY:
		//FinalizeApplication();
		PostQuitMessage(0);
		return 0;

		// This prevents app from freezing when left Alt is pressed
		// (which normally enters modal menu loop).
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return 0;

	case WM_SIZE:
		if ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED))
			//HandlePossibleSizeChange();
			return 0;

	case WM_EXITSIZEMOVE:
		//HandlePossibleSizeChange();
		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			Graphics::GetInstance()->close = true;
			break;
		case 'T':

			break;
		case 'S':

			break;
		}
		return 0;
	case WM_KEYUP:

	case WM_SETCURSOR:	case WM_DEVICECHANGE:
	case WM_MOUSEMOVE:

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



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

void Window::SetWindowHandler(HWND _winWindowHandle)
{
	winWindowHandle = _winWindowHandle;
}
#endif