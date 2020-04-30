#include <memory>
#include <vector>
#include "Graphics.h"
#include "Input.h"

class Window
{
	public:
		Window(int _width, int _height, const char* name);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Graphics& GetGraphics() {
			return *graphics;
		};

		Input input;
	private:
		class WindowClass
		{
		public:
			static const char* GetName();
			static HINSTANCE GetInstance();
		private:
			WindowClass();
			~WindowClass();
			WindowClass(const WindowClass&) = delete;
			WindowClass& operator=(const WindowClass&) = delete;

			static constexpr const char* wndClassName = "Engine Window";
			static WindowClass wndClass;
			HINSTANCE hInst;
		};

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int width = 0;
	int height = 0;
	int x = 0;
	int y = 0;
	int midX = 0;
	int midY = 0;
	HWND hWnd;

	std::vector<BYTE> rawBuffer;

	std::unique_ptr<Graphics> graphics;
};

