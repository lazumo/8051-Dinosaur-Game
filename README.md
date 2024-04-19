# 8051 Dinosaur Game
![image](https://github.com/lazumo/8051-Dinosaur-Game/assets/63379847/9bb521f8-afaf-4d9e-9869-df61330302ed)

This project implements a dinosaur game on the 8051 microcontroller using C. It includes a basic thread manager to handle multiple threads: event detection, game logic processing, and rendering.

## Features

- Basic thread manager for handling multiple threads
- Event detection thread for handling user input and game events
- Game logic processing thread for updating game state and mechanics
- Rendering thread for displaying the game on the LCD
- Utilizes the EdSim51 simulator for 8051 microcontroller development

## Requirements

- EdSim51 simulator
- 8051 microcontroller
- C++ compiler compatible with 8051 (e.g., SDCC)

## Project Structure

- `buttonlib.c`: Library for accessing the button bank in EdSim51
- `dino.c`: Main game implementation, including thread creation and management
- `keylib.c`: Library for working with the keypad
- `lcdlib.c`: Library for controlling the LCD display
- `preemptive.c`: Implementation of the preemptive thread scheduler

## Getting Started

1. Set up the EdSim51 simulator environment.
2. Connect the necessary hardware components (keypad, LCD, buttons) to the 8051 microcontroller.
3. Compile the project files using a C++ compiler compatible with 8051 (e.g., SDCC).
4. Load the compiled program into the 8051 microcontroller using EdSim51.
5. Run the simulator and enjoy playing the dinosaur game!

## Game Instructions

- Press the '#' key to start the game.
- Use the '2' key to make the dinosaur jump and the '8' key to make it crouch.
- Avoid the obstacles (cacti) by jumping or crouching.
- The game ends if the dinosaur collides with an obstacle.
- The score is displayed on the LCD after the game ends.

