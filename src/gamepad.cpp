#include "gamepad.h"

void Gamepad::setup()
{
	
}

void Gamepad::read()
{

	state.dpad = 0;

	state.buttons = 0;

	state.lx = GAMEPAD_JOYSTICK_MID;
	state.ly = GAMEPAD_JOYSTICK_MID;
	state.rx = GAMEPAD_JOYSTICK_MID;
	state.ry = GAMEPAD_JOYSTICK_MID;
	state.lt = 0;
	state.rt = 0;
}
