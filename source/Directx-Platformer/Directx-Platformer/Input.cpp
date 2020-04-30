#include "Input.h"



Input::Input()
{
}


Input::~Input()
{
}

bool Input::IsKeyDown(unsigned char keyCode)
{
	return keyStates[keyCode];
}

void Input::OnKeyDown(unsigned char keyCode)
{
	keyStates[keyCode] = 1;
}

void Input::OnKeyUp(unsigned char keyCode)
{
	keyStates[keyCode] = 0;
}

void Input::ResetAllKeys()
{
	for (int i = 0; i < 256; i++)
	{
		keyStates[i] = 0;
	}

	leftMouseState = mouseState::NONE;
	rightMouseState = mouseState::NONE;
}


void Input::OnMouseMove(float mouseX, float mouseY)
{
	using namespace DirectX;

	mousePos.x = mouseX;
	mousePos.y = mouseY;
	mouseDelta = mousePos;
	mouseDelta.x -= mousePosLastFrame.x;
	mouseDelta.y -= mousePosLastFrame.y;
	mousePosLastFrame = mousePos;
}

void Input::OnLeftMouseDown()
{
	if(leftMouseState == mouseState::DOWN)
		leftMouseState = mouseState::HELD;
	else
		leftMouseState = mouseState::DOWN;
}

void Input::OnRightMouseDown()
{
	if (rightMouseState == mouseState::DOWN)
		rightMouseState = mouseState::HELD;
	else
		rightMouseState = mouseState::DOWN;
}

void Input::OnLeftMouseUp()
{
	leftMouseState = mouseState::UP;
}

void Input::OnRightMouseUp()
{
	rightMouseState = mouseState::UP;
}

void Input::ResetMouseStates()
{
	if (leftMouseState == mouseState::UP)
		leftMouseState = mouseState::NONE;
	else if (leftMouseState == mouseState::DOWN)
		leftMouseState = mouseState::HELD;
	if (rightMouseState == mouseState::UP)
		rightMouseState = mouseState::NONE;
	else if (rightMouseState == mouseState::DOWN)
		rightMouseState = mouseState::HELD;

	mouseDelta = { 0,0};
	rawMouseDelta = { 0,0 };
}