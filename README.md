# Switched-Mode Power Converter Simulator / Emulator

This project implements a **DC power converter simulator/emulator with a digital controller**, developed as part of the course **BL40A1101 Embedded Systems Project (ESP)** at LUT University, School of Energy Systems (Electrical Engineering).

The goal is to design a **real-time control system** consisting of:
- A discrete **converter (plant) model**
- A **PI controller**
- **PWM generation**
- A **user interface** using buttons, LEDs, and UART communication

The system is intended to run on an embedded platform such as **Xilinx Zynq (Zybo)** or **STM32 Nucleo**.

---

## Project Overview

Power converters are typically controlled using DSPs, FPGAs, or microcontrollers. During development, the hardware can be emulated using a real-time plant model to allow controller development without a physical power stage.

In this project:
- A **DC-DC converter model** is simulated using a discrete state-space model.
- A **PI controller** regulates the output voltage.
- The controller output drives a **PWM signal**, visualized using an RGB LED.
- A **user interface** allows changing modes, controller parameters, and reference values using **buttons and UART**.

Bonus tasks extend the system toward inverter operation and IoT connectivity.

---

## Features

### Core (Basic Version)
- Real-time program structure:
  - Polling with interrupts  
  - Custom scheduler **or** FreeRTOS
- Discrete **converter state-space model**
- **PI controller**
- PWM output mapped to RGB LED brightness
- User interface:
  - Buttons
  - LEDs
  - UART (serial console)

### Operating Modes
1. **Configuration Mode**
   - Select and modify controller parameters (`Kp`, `Ki`)
2. **Idling Mode**
   - Converter off, no modulation
3. **Modulating Mode**
   - PI controller regulates output voltage

Mode changes are indicated via LEDs and UART messages.

---

## Converter Model

- Based on a **discrete state-space representation** of an H-bridge converter with EMI filter
- Discretized using **MATLAB `c2d` (impulse method)** at a **50 kHz sampling rate**
- Model states:
  - Inductor currents
  - Capacitor voltages
- Output:
  - Converter output voltage `u_out = u3`

> The implementation uses the provided discrete matrices directly; no modeling derivation is required.

---

## Controller

- Discrete **PI controller**
- Adjustable gains:
  - Proportional gain (`Kp`)
  - Integral gain (`Ki`)
- Gains can be modified via:
  - Buttons
  - UART commands
- Controller output feeds:
  - PWM generator
  - Converter model input

> Controller stability or performance tuning is **not graded**, only correct structure and functionality.

---

## User Interface

### Buttons
- Mode selection
- Parameter selection (`Kp` / `Ki`)
- Increase / decrease values

### UART (Serial Console)
- Change modes
- Set controller parameters
- Set reference voltage
- Print measurements (rate-limited to avoid buffer overflow)

### Synchronization
- Binary semaphores used to:
  - Prevent button interaction during UART configuration
  - Prevent UART changes shortly after button actions
- FreeRTOS users must use **mutex-type semaphores**

---

## Bonus Tasks (Optional)

1. **Inverter Simulation**
   - Single-phase inverter operation
   - 50 Hz sinusoidal output voltage

2. **PWM-Fed Model**
   - PWM output fed directly into the converter model
   - H-bridge behavior emulated using ISR logic

3. **MQTT to Database (Zynq only)**
   - Linux (Ubuntu or OpenAMP)
   - Send data via MQTT to a database (ThingSpeak supported)

4. **OPC UA to Database (Zynq only)**
   - Linux-based OPC UA server
   - Data transmission to external database

> Maximum of **two bonus tasks** can be graded.

---

## Evaluation Criteria

- Correct functionality of controller and model
- Clean, readable, well-commented code
- Structured program design
- Proper real-time scheduling
- Correct use of semaphores
- Functional UI (buttons + UART)
- Report quality and demonstration video

A short **video demonstrating the system** is strongly encouraged.

---

## Deliverables

- Source code
- Short description of the implementation
- (Optional but recommended) demonstration video

### Deadlines
- **31.1.2025** – Full points
- **28.2.2025** – Reduced points
- **30.4.2025** – Absolute final deadline

---

## Hardware Platforms

- **Xilinx Zynq (Zybo)**
- **STM32 Nucleo**
  - May require external LEDs and switches for full functionality

---

## Notes

- Do **not** use blocking delays (`sleep`, busy loops) outside initialization
- Use interrupts or RTOS scheduling for real-time behavior
- UART output must be rate-limited (≈0.1–1 s)

---

## Course Information

- Course: **BL40A1101 Embedded Systems Project**
- Institution: LUT University, School of Energy Systems
- Date: 11.10.2024

---