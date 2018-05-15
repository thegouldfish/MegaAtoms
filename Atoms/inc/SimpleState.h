#ifndef SIMPLE_STATE
#define SIMPLE_STATE


typedef void _voidCallBack();

typedef struct
{
	_voidCallBack *Start;

	_voidCallBack *Update;

	_voidCallBack *End;

	int TransisionIn;

	int TransisionOut;
} SimpleState;



typedef struct
{
	int TransisionOutFrames;
	int TransisionInFrames;

	SimpleState* CurrentState;
	SimpleState* ChangeTo;

} StateMachine;



void StateMachineStart(StateMachine* machine, SimpleState* state);
void StateMachineChange(StateMachine* machine, SimpleState* state);
void StateMachineUpdate(StateMachine* machine);

#endif