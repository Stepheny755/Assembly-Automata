# Assembly-Automata
Conway's Game of Life on the De1-SoC computer's VGA.  
**Authors:** Stephen Yang and Yvonne Yang  
![ece243_life](https://user-images.githubusercontent.com/60620007/114575657-6326d600-9c48-11eb-811a-7c1b3d6f0779.gif)

## Introduction
For the longest time, humans believed that complex behavior only results from complex rules, and that a machine that can build itself is merely fantastical. Along came John Conway, with 3 simple rules that define a game (or cellular automaton) with astonishing complexity. The game is to be played with nothing but an initial configuration of tiles on a board, which evolves on its own based on 3 rules. The rules are these:  

1. Underpopulation: A tile that is alive dies if it has less than 2 neighbors
2. Overpopulation: A tile that is alive dies if it has more than 3 neighbors
3. Reproduction: A dead tile revives if it has exactly 3 neighbors

Welcome to the world of Game of Life. Welcome to the world of oscillators, spaceships, glider guns and caterloopillars. There have been communities founded for the sole purpose of investigating these creatures. As you will see in the simulations, most configurations die off after some time, but some persist, some grow indefinitely. How do we determine if a given configuration vanishes? After a thousand iterations? After a million iterations? Despite the seeming simplicity of this problem, nobody has a good answer. Conway’s Game of Life is also Turing Complete. This means that we can build AND gates, OR gates and NOT gates, and computers from these. We can even build Game of Life in Game of Life!  

We hope you enjoy exploring Game of Life with our program.  

## Screens
![image](https://user-images.githubusercontent.com/60620007/114571482-b3039e00-9c44-11eb-98dd-6b506f2b7c71.png)
![image](https://user-images.githubusercontent.com/60620007/114571531-bbf46f80-9c44-11eb-8533-5752badf05e3.png)
![image](https://user-images.githubusercontent.com/60620007/114571584-c878c800-9c44-11eb-8d1c-153a50fcbaf0.png)
![image](https://user-images.githubusercontent.com/60620007/114571622-d2023000-9c44-11eb-9452-b823f34e359a.png)


## How to run the program
Download main.c. Go to https://cpulator.01xz.net/. Select ARMv7 and DE1-SoC for architecture, then under language, select C. Open main.c, press Compile and Load, and press continue.  
The program consists of 4 screens shown on the VGA display, the main menu, a randomly generated board, a gallery of preset creatures, and a board to draw your own. Pushbutton 0 always starts/pauses the Life simulation, whereas pushbuttons 1-3 help navigate between the pages. The mouse can also be used for navigation.  

---

R.I.P. John Conway, 1927-2020
