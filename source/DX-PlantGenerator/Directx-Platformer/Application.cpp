#include "Application.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <string>

Application::Application(int width, int height, const char* name) 
	: window(width, height, name)
{
//#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
//	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
//	if (FAILED(initialize))
//		// error
//#else
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
//	if (FAILED(hr))
//		// error
//#endif

	window.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)window.GetGraphics().GetWindowHeight() / (float)window.GetGraphics().GetWindowWidth(), 0.5f, 400.0f));
	window.GetGraphics().GetCamera().SetPosition({ 0,0,-2 });

	serviceLocator.instance = &serviceLocator;
	serviceLocator.sceneManager = &sceneManager;
	serviceLocator.input = &window.input;
	serviceLocator.camera = &window.GetGraphics().GetCamera();
	serviceLocator.time = &time;
}


Application::~Application()
{
}

int Application::Start()
{
	D3D11_FEATURE_DATA_THREADING treadData;
	window.GetGraphics().GetDevice()->CheckFeatureSupport(D3D11_FEATURE_THREADING, &treadData, sizeof(treadData));

	mainApp.OnStart();

	while (true)
	{
		int result = ProcessMessages();
		if (result != 0)
			return result;
		Tick();
	}
}

int Application::ProcessMessages()
{
	window.input.ResetMouseStates();
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		// Generates WM_CHAR messages from valid charater keyboard inputs
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (window.input.IsKeyDown(VK_MENU))
		{
			//MessageBox(nullptr, "A Pressed!", "You pressed A.", MB_OK | MB_ICONEXCLAMATION);
		}
	}
	return 0;
}

void Application::Tick()
{
	window.GetGraphics().GetCamera().hasMoved = false;
	time.Update();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	window.GetGraphics().ClearBuffer(1, 1, 1);
	mainApp.OnUpdate();

	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	window.GetGraphics().EndFrame();
}
