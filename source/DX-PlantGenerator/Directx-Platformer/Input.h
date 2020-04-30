#pragma once
#include <bitset>
#include <DirectXMath.h>

class Input
{
	friend class Window;
	friend class Application;
public:
	Input();
	~Input();

	bool IsKeyDown(unsigned char keyCode);

	DirectX::XMFLOAT2 GetMousePos() { return mousePos; };
	DirectX::XMFLOAT2 GetMouseDelta() { return mouseDelta; };
	DirectX::XMFLOAT2 GetRawMouseDelta() { return rawMouseDelta; };

	bool IsLeftMouseButtonDown() { return leftMouseState == mouseState::DOWN; } // Returns true the frame the left mouse button is clicked
	bool IsLeftMouseButtonHeld() { return leftMouseState == mouseState::HELD || leftMouseState == mouseState::DOWN; } // Returns true every frame the left moues button is held down
	bool IsLeftMouseButtonUp() { return leftMouseState == mouseState::UP; } // Returns true the frame the left mouse button is released

	bool IsRightMouseButtonDown() { return rightMouseState == mouseState::DOWN; } // Returns true the frame the right mouse button is clicked
	bool IsRightMouseButtonHeld() { return rightMouseState == mouseState::HELD || rightMouseState == mouseState::DOWN; } // Returns true every frame the right moues button is held down
	bool IsRightMouseButtonUp() { return rightMouseState == mouseState::UP; } // Returns true the frame the right mouse button is released

private:
	void ResetMouseStates();

	enum class mouseState
	{
		NONE,
		DOWN,
		HELD,
		UP
	};

	DirectX::XMFLOAT2 mousePos = { 0,0 };
	DirectX::XMFLOAT2 mousePosLastFrame = { 0, 0 };
	DirectX::XMFLOAT2 mouseDelta = { 0,0 };

	DirectX::XMFLOAT2 rawMouseDelta = { 0, 0 };

	mouseState leftMouseState = mouseState::NONE;
	mouseState rightMouseState = mouseState::NONE;

	void OnKeyDown(unsigned char keyCode);
	void OnKeyUp(unsigned char keyCode);
	void ResetAllKeys();

	void OnMouseMove(float mouseX, float mouseY);
	void OnRawMouseDelta(float xDelta, float yDelta) { rawMouseDelta = { xDelta, yDelta }; };
	void OnLeftMouseDown();
	void OnRightMouseDown();
	void OnLeftMouseUp();
	void OnRightMouseUp();

	std::bitset<256u> keyStates;
};

