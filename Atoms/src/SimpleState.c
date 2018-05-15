#include "..\inc\SimpleState.h"



void StateMachineStart(StateMachine* machine, SimpleState* state)
{
	if (state != 0 && machine != 0)
	{
		machine->ChangeTo = 0;
		machine->TransisionOutFrames = -1;
		machine->TransisionInFrames = -1;

		machine->CurrentState = state;
		machine->CurrentState->Start();
	}
}


void StateMachineChange(StateMachine* machine, SimpleState* state)
{
	if (state != 0 && machine != 0)
	{
		machine->ChangeTo = 0;

		machine->CurrentState->End();

		machine->CurrentState = state;
		
		machine->CurrentState->Start();
	}
}


void StateMachineUpdate(StateMachine* machine)
{
	if (machine != 0)
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
