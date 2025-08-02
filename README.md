
# bowlermakes ece3620 project

A group project for Purdue’s Fall 2024 ECE3620 Embedded Systems course. This project implements a bowling game on custom hardware using an STM32F091RCT6 microcontroller.

https://github.com/user-attachments/assets/6cc2ac29-b5b4-42c4-b31b-98f5b0218610

## Hardware Overview

- **MCU:** STM32F091RCT6
- **Sensor:** AK9753 NDIR sensor (gesture input)
- **Display:** MSP2202 LCD (CR2013-MI2120, ILI9341 controller)
- **EEPROM:** 24AA32AF (24LC32)
- **Buttons:** Tactile switches (SW_MEC_5E)
- **Power:** 3.3V

Refer to the KiCad schematic at `KiCadProject-Bowlermakers/KiCadProject-Bowlermakers.kicad_sch` for wiring and connections.

## Project Structure

```text
/
├── Bowlermakers-Code/
│   ├── src/            # Main application and modules
│   ├── include/        # Header files
│   ├── lib/            # External libraries (Empty)
│   ├── test/           # Test files (Empty)
│   └── platformio.ini  # PlatformIO configuration
├── KiCadProject-Bowlermakers/
│   ├── KiCadProject-Bowlermakers.kicad_sch  # Schematic
│   ├── KiCadProject-Bowlermakers.kicad_pcb  # PCB layout
│   └── ...
└── ...
```

## Gesture Detection

Gesture detection is implemented using the AK9753 NDIR sensor and a custom signal processing algorithm:

1. **Sensor Data Acquisition:** The AK9753 sensor is read via I2C. Four IR channels (IR1–IR4) provide distance measurements, which are processed in real time.
2. **Preprocessing:** Each IR value is compared to a threshold (`DIST_CUTOFF`). If above threshold, it is marked as "active" and stored in a circular history buffer for each channel.
3. **Swipe Detection:** When the buffer is full, the code checks for swipe gestures using a convolution-based algorithm:
   - The function `parse_conv_arr()` convolves the history arrays with a kernel `[1, -1]` to detect transitions.
   - If a transition is detected (output of convolution is `1`), it identifies a swipe direction (left or right).
   - The code distinguishes between vertical and horizontal swipes using the `vertSwipe` flag and different sensor pairs.
4. **Debouncing:** After a swipe is detected, a cooldown (`ir_cooldown_flag`) prevents repeated detections until the buffer cycles.
5. **Result:** The detected swipe direction is stored in `current_swipe`, which the main application uses to control game navigation and actions.

This approach enables robust, touchless gesture detection using simple signal processing on the IR sensor data.

## Gameplay

- **Startup Screen:** Use gesture controls (swipe up/left/right/down) to navigate, functioning like a D-Pad.
- **Game Flow:**
  - Swipe left/right to set ball position.
  - Press button to confirm.
  - Swipe left/right to set ball angle.
  - Press button to bowl.
- Pins and scores are displayed on the LCD.

## Code Structure

- `src/` — Main application and modules:
  - `main.c` — Entry point and state machine
  - `game_logic.*` — Bowling game logic
  - `graphics.*` — LCD graphics
  - `physics.*` — Ball movement and collision
  - `scoring.*` — Score calculation
  - `pins.*` — Pin positions and hit detection
  - `device_drivers/` — Sensor, LCD, EEPROM, IR, GPIO, I2C, UART drivers

## Building & Flashing

This is a PlatformIO project.  
To build and upload the firmware:

```sh
platformio run --target upload
```

## Usage

1. Assemble hardware per the KiCad schematic.
2. Flash the firmware using PlatformIO.
3. Power on the board and use gestures/buttons to play.
