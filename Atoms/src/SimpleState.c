#include <genesis.h>
#include "..\inc\SimpleState.h"


static int Switching = 0;

void StateMachineStart(StateMachine* machine, SimpleState* state)
{
	if (state != 0 && machine != 0)
	{
		Switching = 1;
		machine->ChangeTo = 0;
		machine->TransisionOutFrames = -1;
		machine->TransisionInFrames = -1;

		machine->CurrentState = state;
		machine->CurrentState->Start();
		Switching = 0;
	}
}


void StateMachineChange(StateMachine* machine, SimpleState* state)
{
	if (state != 0 && machine != 0 && Switching == 0)
	{
		Switching = 1;
		machine->ChangeTo = 0;

		machine->CurrentState->End();

		SPR_update();
		VDP_waitVSync();

		machine->CurrentState = state;
		
		machine->CurrentState->Start();
		Switching = 0;
	}
}


void StateMachineUpdate(StateMachine* machine)
{
	if (machine != 0 && Switching == 0)
	{
		machine->CurrentState->Update();

		/*
		if (machine->ChangeTo != 0)
		{
			if (machine->TransisionInFrames > 0)
			{
				machine->TransisionInFrames--;
			}
			else if (machine->TransisionInFrames == 0)
			{
				machine->CurrentState = machine->ChangeTo;
				machine->TransisionInFrames--;
				machine->ChangeTo = 0;
			}

			if (machine->TransisionOutFrames > 0)
			{
				machine->TransisionOutFrames--;
			}
			else if (machine->TransisionOutFrames = 0)
			{
				machine->ChangeTo->Start();
				machine->TransisionOutFrames--;			
			}			
		}
		else
		{
			
		}
		*/
	}
}


void StateMachineVInterrupt(StateMachine* machine)
{
	if (machine->CurrentState->VInterrupt != NULL)
	{
		machine->CurrentState->VInterrupt();
	}
}