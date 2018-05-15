#include <genesis.h>

#include "gfx.h"
#include "sprite.h"
#include "sound.h"

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
    
	while(TRUE)
    {
        SPR_update();
		
		StateMachineUpdate(&GameMachineState);

        VDP_waitVSync();
    }

    return 0;
}