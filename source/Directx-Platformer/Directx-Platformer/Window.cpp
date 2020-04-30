#include "Window.h"
#include "MeshRenderer.h"
#include "imgui.h"
#include "imgui_impl_win32.h"


Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass()
	: hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc;
	// Create the window struct
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = wndClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, hInst);
}

HINSTANCE Window::WindowClass::GetInstance()
{
	return wndClass.hInst;
}

const char* Window::WindowClass::GetName()
{
	return wndClassName;
}

// Since the window api doesn't allow using a member object function for its WNDPROC function, we need to retreive a reference 
// to the window object using a static function first to then to store it for later use. 
LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Just before the window is created
	if (msg == WM_NCCREATE)
	{
		// Cast lParam to the correct struct for WM_NCCREATE events
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		// The parameter lpCreateParams is a pointer to window class that was passed in with CreateWindowEx, so we can safley static cast to that
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// Using this method we can store a reference to the window instance for later use
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// Switch out the function for WINDPROC to use HandleMsgThunk instead now that we have the pointer to our window instance
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// We can now send this data to our non-static message handler
		pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// Let the default message handler handle any messages before WM_NCCREATE
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// This function takes windows messages and sends them to our window member function
LRESULT Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Retreive the pointer to our member function which was saved in HandleMsgSetup
	Window* const win = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return win->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(-1);
		return 0;
	}
	case WM_KEYDOWN:
	{
		input.OnKeyDown(wParam);
		break;
	}
	case WM_KEYUP:
	{
		input.OnKeyUp(wParam);
		break;
	}
	case WM_SYSKEYDOWN:
	{
		input.OnKeyDown(wParam);
		break;
	}
	case WM_SYSKEYUP:
	{
		input.OnKeyUp(wParam);
		break;
	}
	case WM_KILLFOCUS:
	{
		input.ResetAllKeys();
		//ReleaseCapture();
		break;
	}
	case WM_SETFOCUS:
	{
		//SetCapture(hWnd);
		break;
	}
	case WM_MOUSEMOVE:
	{
		POINTS pt = MAKEPOINTS(lParam);

		if (pt.x != midX && pt.y != midY)
		{
			input.OnMouseMove(pt.x, pt.y);
			SetCursorPos(midX, midY);
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		input.OnLeftMouseDown();
		break;
	}
	case WM_LBUTTONUP:
	{
		input.OnLeftMouseUp();
		break;
	}
	case WM_RBUTTONDOWN:
	{
		input.OnRightMouseDown();
		break;
	}
	case WM_RBUTTONUP:
	{
		input.OnRightMouseUp();
		break;
	}
	case WM_WINDOWPOSCHANGED:
	{
		WINDOWPOS* winInfo = reinterpret_cast<WINDOWPOS*>(lParam);
		width = winInfo->cx;
		height = winInfo->cy;
		x = winInfo->x;
		y = winInfo->y;

		midX = x + ((float)width / 2.0f);
		midY = y + ((float)height / 2.0f);
		break;
	}
	case WM_INPUT:
	{
		// Get the size of the input data
		UINT size = 0;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER));

		rawBuffer.resize(size);

		// Read in the data
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER));

		// Process the data
		auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE && (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
		{
			input.OnRawMouseDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(int _width, int _height, const char* name)
{
	width = _width;
	height = _height;
	// Adjust the window size so the passed width/height represent the user area rather than the full window
	RECT rect;
	rect.left = 100;
	rect.right = width + rect.left;
	rect.top = 100;
	rect.bottom = height + rect.top;

	AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	// Create the window
	HWND hWnd = CreateWindowEx(
		0,
		WindowClass::GetName(),
		"Procedural Open World",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	ShowWindow(hWnd, SW_SHOW);

	ImGui_ImplWin32_Init(hWnd);
	
	graphics = std::make_unique<Graphics>(hWnd, width, height);
	MeshRenderer::SetGraphicsDevice(graphics.get());

	// Register mouse as raw input device
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;

	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		throw;
	}
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}
