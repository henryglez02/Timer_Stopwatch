# Timer_Stopwatch
PIC Microcontroller Timer/Stopwatch Project

This project implements a timer/stopwatch for PIC microcontrollers, developed using MPLAB IDE and the HI-TECH C Compiler, and simulated in Proteus.

## Description

The project offers two modes of operation:
- **Stopwatch:** Counts time upward.
- **Timer:** Counts down from a preset time, activating an LED and a buzzer when the timer reaches zero.

The time is displayed on a 16x2 LCD in MM:SS format. User interaction is provided via buttons to start/stop, change mode, reset, and adjust the time in timer mode.

## Tools Used

- **Proteus:** For circuit simulation.
- **MPLAB IDE:** Integrated development environment for PIC microcontrollers.
- **HI-TECH C Compiler:** Compiler used for C programming.

## Features

- **Operational Modes:** Choose between stopwatch and timer modes.
- **Visual Interface:** Time is displayed on a 16x2 LCD screen.
- **User Control:** Buttons for starting/stopping, changing mode, resetting, and adjusting the time.
- **Indicators:** LED and buzzer alert when the timer reaches zero.
- **Interrupt Management:** Utilizes Timer1 to generate 1-second intervals.

## Hardware and Software Requirements

### Hardware

- PIC Microcontroller (e.g., PIC16F877A or another compatible device).
- 16x2 LCD Display.
- Push buttons for user input.
- LED and buzzer.
- Passive components (resistors, capacitors, etc.).
- Testing platform (breadboard or PCB).
- Proteus simulation (optional).

### Software

- MPLAB IDE.
- HI-TECH C Compiler.
- Proteus Design Suite (for circuit simulation).

## How to Use

1. **Proteus Simulation:**
   - Open the Proteus project (e.g., `.DSN` or `.PRO` file).
   - Verify the circuit connections (LCD, buttons, LED, buzzer, and microcontroller).
   - Run the simulation to test the timer/stopwatch functionality.

2. **Compilation and Programming:**
   - Open the project in MPLAB IDE.
   - Ensure the HI-TECH C Compiler is configured correctly.
   - Compile the source code (`main.c`).
   - Program the microcontroller using an appropriate programmer (e.g., PICkit).
