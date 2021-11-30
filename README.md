# MegaAtoms
Atoms for the MegaDrive (Genesis)!
![](https://www.dropbox.com/s/ljam6tgxphunlve/MegaAtoms.png?raw=1)

This is the source code and release of MegaAtoms, which is documented as part of a youtube series
[GouldFish On Games](https://www.youtube.com/watch?v=Mh6SBBHMiFI)

This is using [SGDK](https://github.com/Stephane-D/SGDK)


# About

Mega Atoms contains two game modes, Classic and Challenge



# Classic
Atoms is 6 player multi-player game in which you have to create take control of the playing board.
you can play it in hot seat mode against 5 other people, or against (quite stupid) AI players.

![](https://www.dropbox.com/s/qgvbkfr6gyndkpt/playerselect.png?raw=1)

 
# How To Play

Place your atoms on any empty square or stack them on top of your own, but when the numbers get too high they will go critical! When it explodes it will send atoms flying and end up stacking on the atoms around it, which not long increases the size of the stack but also put the stack under your control.  This is the key to taking over the gaming board.

![](https://www.dropbox.com/s/07yj78wr85o0bo9/ingame.png?raw=1)

## Input

Currently the game is playable only using the first controller.
Press start once you've setup the game then press A to place Atoms.



## Challenge
This uses the same basic mechanics of classic mode but now you need to pop a set number of atoms to make it to the next level.
you gain points and more time from popping the atoms and the goal is to get the best score possible.
The score multipler increases for each chain reaction you start but is capped at the current level so moving up the levels
are required to get a better score.

## How to play
Basic game play is the same as classic, but you have to place the Atom provided.
If you clear the screen (set it to the same colour) or don't have a space to put your current atom you will get
a time penalty.




# History


## V1.0
Challenge mode reblanced
Screen updates made faster
Music should no longer pause.
Various tweaks and improvements to the code


## V0.4.1
Fixed crashing when using a 4mb cart to play the game.
Code that needs to happen in a v-blank can now do so easily
Game should play correctly when the hardware resets.
Move most of the states over to using the global pad struct.


## V0.4
Pause Screen added to Classic and Challenge
High score table added to Challenge
High scores saved to sram
Progress bars added to Classic
New artwork for professors and robots for Classic mode
Tutorial added to Challenge mode


#V0.3.1
Bug fix release. 
Tutorial works again.
Can play challenge mode multiple times.
Graphical corruption fixes.


## V0.3
Challenge mode added
new Animation for the title screen
Updated Character art


## V0.2
Idle Animations added
Game cursor returns to the last place a player was
Player select screen improvements
Numerous bug fixes


## V0.1
First release, included classic mode.

