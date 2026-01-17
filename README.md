# MLC-FWN51 - Touch Controller with IR Remote

## Overview
This project is a **dual-channel LED light controller** featuring:
- **NEC IR Remote Control** - Control brightness, CCT, and presets wirelessly
- **DWIN Touch Display** - Communicate via UART for user interface
- **PWM LED Control** - Independent White and Yellow LED channels for CCT mixing
- **Memory Presets** - Endo, MemOne, MemTwo, and Max brightness modes

## Target MCU
**Nuvoton MS51FB9AE**
- 8051-based microcontroller
- 24 MHz HIRC (High-speed Internal RC oscillator)
- 16KB Flash, 768 bytes XRAM
- Enhanced PWM, UART, Timer peripherals

## BSP (Board Support Package)
This project uses the **official Nuvoton MS51 BSP V2.0** from GitHub for maximum compatibility and maintainability.

- **Source**: [OpenNuvoton/MS51_BSP](https://github.com/OpenNuvoton/MS51_BSP)
- **Version**: BSP V2.0
- **License**: Apache-2.0
- **BSP Variant**: `MS51FB9AE_MS51XB9AE_MS51XB9BE`

### BSP Features
- Compiler-independent header system (`numicro_8051.h` auto-selects Keil/IAR/SDCC)
- Complete SFR definitions and bit manipulation macros
- Standard Driver library for peripherals (UART, PWM, Timer, etc.)
- Official tested and maintained by Nuvoton

## Project Structure
```
MLC-FWN51/
├── include/                      # Application header files
│   └── ir_rx.h                  # IR receiver API
├── src/                          # Application source files
│   ├── main.c                   # Main application
│   └── ir_rx.c                  # NEC IR receiver
├── Library/                      # Official Nuvoton MS51 BSP V2.0
│   ├── Device/Include/          # MCU-specific definitions
│   │   ├── numicro_8051.h       # Main entry point (auto-selects compiler)
│   │   ├── ms51_16k_keil.h      # Keil C51 SFR definitions
│   │   ├── ms51_16k_iar.h       # IAR SFR definitions
│   │   ├── ms51_16k_sdcc.h      # SDCC SFR definitions
│   │   ├── sfr_macro_ms51_16k.h # Bit manipulation macros
│   │   └── function_define_ms51_16k.h  # GPIO, Timer, PWM macros
│   └── StdDriver/               # Peripheral driver library
│       ├── inc/                 # Driver headers (18 files)
│       └── src/                 # Driver sources (19 files)
├── startup/                      # Startup files
│   └── STARTUP.A51              # Official BSP 8051 startup
├── .vscode/                      # VS Code configuration
│   ├── c_cpp_properties.json
│   ├── settings.json
│   ├── tasks.json
│   └── launch.json
├── MLC-FWN51.uvprojx            # Keil µVision project
└── MLC-FWN51.code-workspace     # VS Code workspace
```

## Hardware Configuration

### Pin Assignment
| Pin | Function | Description |
|-----|----------|-------------|
| P0.4 | Buzzer | Audio feedback output |
| P0.5 | IR_RX | IR receiver input (38kHz) |
| P0.6 | UART0_TXD | DWIN display TX |
| P0.7 | UART0_RXD | DWIN display RX |
| P1.4 | PWM1 | White LED channel |
| P1.5 | PWM5 | Yellow LED channel |

### IR Remote Commands (NEC Protocol)
| Button | Address | Command | Inverted | Function |
|--------|---------|---------|----------|----------|
| Power | - | 0x81 | 0x7E | Toggle power on/off |
| White+ | - | 0xA1 | 0x5E | Increase brightness |
| White- | - | 0x51 | 0xAE | Decrease brightness |
| Yellow | - | 0x99 | 0x66 | Cycle CCT level |
| Endo | - | 0xF9 | 0x06 | Endo preset mode |
| Mem1 | - | 0x41 | 0xBE | Memory preset 1 |
| Max | - | 0xD9 | 0x26 | Maximum brightness |
| Mem2 | - | 0xC1 | 0x3E | Memory preset 2 |

### DWIN Display VP Addresses
| Address | Description |
|---------|-------------|
| 0x1000 | Power status |
| 0x1100 | Brightness level (0-10) |
| 0x1200 | CCT level (0-10) |
| 0x1300 | MemOne trigger |
| 0x1400 | MemTwo trigger |
| 0x1600 | Endo/Max trigger |
| 0x2000 | Screen control |

## Building the Project

### Using Keil µVision
1. Open `MLC-FWN51.uvprojx` in Keil µVision IDE
2. Build the project (F7)
3. Flash to target MCU

**Project Configuration:**
- Device: MS51FB9AE
- Include Paths: `.\include;.\Library\Device\Include;.\Library\StdDriver\inc`
- Memory Model: Small
- Code ROM Size: Large
- XDATA: 0x0000 - 0x02FF

### Using SDCC (Open Source)
```bash
sdcc -mmcs51 --model-small --stack-auto \
    -I./include \
    -I./Library/Device/Include \
    -I./Library/StdDriver/inc \
    src/main.c src/ir_rx.c \
    Library/StdDriver/src/common.c \
    Library/StdDriver/src/delay.c \
    -o MLC-FWN51.ihx
```

### Updating the BSP
To update to a newer BSP version:
1. Clone the latest BSP: `git clone https://github.com/OpenNuvoton/MS51_BSP.git`
2. Copy files from `MS51_Series_BSP/MS51FB9AE_MS51XB9AE_MS51XB9BE/Library/` to `./Library/`
3. Verify compatibility with existing code

## Porting Notes (N76E003 → MS51FB9AE)

### Clock Frequency Change
- **N76E003**: 16 MHz HIRC
- **MS51FB9AE**: 24 MHz HIRC

### Timer Calculations
Timer tick = 12 / Fsys
- N76E003: 12/16MHz = 0.75µs per tick
- MS51FB9AE: 12/24MHz = 0.5µs per tick

All timing values scaled by factor of 1.5x.

### UART Baud Rate
```c
// N76E003 @ 16MHz
TH1 = 256 - (1000000 / baudrate);

// MS51FB9AE @ 24MHz  
TH1 = 256 - (1500000 / baudrate);
```

### IR Timing Thresholds
| Parameter | N76E003 (ticks) | MS51FB9AE (ticks) |
|-----------|-----------------|-------------------|
| sync_high | 17000 | 25500 |
| one_high | 3600 | 5400 |
| zero_high | 1800 | 2700 |

## License
SPDX-License-Identifier: Apache-2.0

Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.

## Version History
- **v1.1.0** (2025-01-17): Integrated official Nuvoton MS51 BSP V2.0
- **v1.0.0** (2025-01-17): Initial port from N76E003 to MS51FB9AE
