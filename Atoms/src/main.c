#include <genesis.h>

#include "../inc/PadHelper.h"
#include "../inc/atoms.h"

#include "../inc/Title.h"
#include "../inc/GameState.h"
#include "../inc/AtomsGameState.h"


StateMachine GameMachineState;

int main()
{

	
    // disable interrupt when accessing VDP
    SYS_disableInts();

    // initialization
    VDP_setScreenWidth320();

    // init sprites engine
    SPR_init(80, 0, 0);

    SYS_enableInts();

	StateMachineStart(&GameMachineState, &TitleState);
	//StateMachineStart(&GameMachineState, &AtomsGameState);
    
	SetupPad(&m_Pad, JOY_1);

	while(TRUE)
    {
        
		
		UpdatePad(&m_Pad);
		StateMachineUpdate(&GameMachineState);

		SPR_update();
        VDP_waitVSync();
    }

    return 0;
}