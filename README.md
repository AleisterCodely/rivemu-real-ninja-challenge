# Real Ninja Challenge
![image](https://github.com/AleisterCodely/rivemu-real-ninja-challenge/assets/67978431/1ca349fb-42ef-4cbd-b0d0-dc35c8238118)

A game made for the [rives.io](https://rives.io/) [RISC-V Retro Jam #1](https://itch.io/jam/rives1) bottom-up with C and Spritework by me (I'm no artist!)

# Gameplay
Press A or START on your gamepad to begin (Z on PC), then match the directions on-screen with your Dpad, I previously mentioned in writing that this was pretty much like calling stratagems down in helldivers.

# Bugs
One known bug so far that I'm able to replicate pretty consistently and haven't been able to fix, if you mash the directions while playing it's possible to cause the game to segfault, quitting out of the blue without explanation.

# Building

Copy and paste the following command on the folder where you've downloaded the source code, notice this repo already comes with a .sqfs prebuilt, so you should only build another one once you've made changes to the code

`rivemu -quiet -no-window -sdk -workspace -exec riv-mksqfs 0-entry.sh realninjachallenge.c splash.png slash_up.png slash_down.png slash_left.png slash_right.png arrow_down.png arrow_left.png arrow_right.png arrow_up.png miss.png APPLE.png BANANAS.png GRAPES.png PEACH.png realninjachallenge.sqfs` 
