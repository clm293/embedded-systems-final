# embedded-systems-final
A final project for ECE/CS 3140 at Cornell.
By Christine Mayer and Carly Swetz
May 2019

This project is a two-player reaction time game programmed on the the NXP FRDM-K64F board.

Game play:
Our final project is a two-player reaction time game that uses two buttons and the LED. The game begins when the RESET button is pressed. Before each round, the green LED flashes three times to signal to the players that the white LED is coming. After the green LED blinks three times, the white LED will come on in anywhere from 10 to 50 tenths of a second (1 to 5 seconds). The first player to press their respective button wins that round and their color will flash. Another round then starts by first blinking green three times. The game will continue until a player has won three rounds. We also added a false start feature to our game. If a player is too eager and presses their button before the white LED comes on, they will lose that round and give the other player a point. We assume that a player will not false start during the three green flashes. As soon as a player has won the game, their color LED will stay on.
