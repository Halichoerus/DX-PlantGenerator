#include <Windows.h>
#include <string>

#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(1);
			break;
		}
		case WM_KEYDOWN:
		{
			if (wParam == 'F')
			{
				SetWindowText(hWnd, "F test");
			}
			break;
		}
		case WM_KEYUP:
		{

			break;
		}
		case WM_CHAR:
		{
			static std::string title;
			title.push_back(wParam);
			SetWindowText(hWnd,title.c_str());
			break;
		}
		case WM_LBUTTONDOWN:
		{
			POINTS point = MAKEPOINTS(lParam);
			std::string title = "(" + std::to_string(point.x) + "," + std::to_string(point.y) + ")";
			SetWindowText(hWnd,title.c_str());
			break;
		}
		default:
		{
			break;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Application app(1280, 720, "Plant Generator");
		return app.Start();
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "An error in window creation occured", MB_OK | MB_ICONEXCLAMATION);
	}
}