#include "../inc/PadHelper.h"


u8 UpdateInput(u16 newState, u16 prev, u16 toCheck)
{
	if ((newState & toCheck) && (prev & toCheck))
		return PAD_HELD;
	else if (!(prev & toCheck) && (newState & toCheck))
		return PAD_PRESSED;
	else if ((prev & toCheck) && !(newState & toCheck))
		return PAD_RELEASED;
	else
		return PAD_UP;

}


void SetupPad(Pad* pad, u16 id)
{
	pad->Id = id;
	pad->PrevState = 0;
	pad->CurrentState = 0;

	UpdatePad(pad);
}

void ResetPad(Pad* pad)
{
	pad->PrevState = 0;
	pad->CurrentState = 0;

	UpdatePad(pad);
}

void UpdatePad(Pad* pad)
{
	u16 value = JOY_readJoypad(pad->Id);	
	
	pad->PrevState = pad->CurrentState;
	pad->CurrentState = value;

	if (pad->Id == JOY_TYPE_MOUSE)
	{


	}

	pad->A = UpdateInput(pad->CurrentState, pad->PrevState, BUTTON_A);
	pad->B = UpdateInput(pad->CurrentState, pad->PrevState, BUTTON_B);
	pad->C = UpdateInput(pad->CurrentState, pad->PrevState, BUTTON_C);
	pad->START = UpdateInput(pad->CurrentState, pad->PrevState, BUTTON_START);

	pad->Up = UpdateInput(pad->CurrentState, pad->PrevState, BUTTON_UP);
	pad->Down = UpdateInput(pad->CurrentState, pad->PrevState, BUTTON_DOWN);
	pad->Left = UpdateInput(pad->CurrentState, pad->PrevState, BUTTON_LEFT);
	pad->Right = UpdateInput(pad->CurrentState, pad->PrevState, BUTTON_RIGHT);
}

void UpdateMouse()
{
	
}