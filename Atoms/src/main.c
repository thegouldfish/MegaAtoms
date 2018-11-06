#include <genesis.h>
#include <sram.h>

#include "../inc/PadHelper.h"
#include "../inc/atoms.h"

#include "../inc/Title.h"
#include "../inc/GameState.h"
#include "../inc/AtomsGameState.h"

#include "../inc/Prototype2.h"
#include "../inc/ChallengeModeSetNameState.h"
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
    	
	SetupPad(&m_Pad, JOY_1);

	DefaultScores();

	
	SRAM_enable();
	if (!LoadScores())
	{
		// if we fail to load the score try to save out the defaults.
		SaveScores();
	}


#if DEBUG
	m_Score = 20001;
	StateMachineStart(&GameMachineState, &ChallengeModeSetNameState);
#else
	StateMachineStart(&GameMachineState, &TitleState);
#endif

	while(TRUE)
    {
        
		
		UpdatePad(&m_Pad);
		StateMachineUpdate(&GameMachineState);

		SPR_update();
        VDP_waitVSync();
    }

    return 0;
}