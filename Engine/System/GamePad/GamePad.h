#pragma once
#include <Windows.h>
#include "Xinput.h"
#include "Vector2.h"
#pragma comment(lib, "xinput.lib")

class GamePad
{
public:
	
	void Initialize();

	void Update();

private:

	Vector2 padMotorRange = { 1.0f,1.0f };

	struct GamePadState {
		bool connected;
		float leftStickX;
		float leftStickY;
		float rightStickX;
		float rightStickY;
		float leftTrigger;
		float rightTrigger;
		bool buttons[14]; // A,B,X,Y,LB,RB,Back,Start,LS,RS,DPAD(↑↓←→)
	};
};