# Continuous Lab Evaluation-5B.2

**EXP:** Continuous Lab Evaluation-5B.2  
**DATE:** 01/09/2026  
**NAME OF EXP:** Hardware Timer and Interrupt-Based Timing — Variable-Frequency Square-Wave Generation 
**HARDWARE SETUP / PLATFORM:** Keil C51 + AMOTECH LABS 8051 Development Board / UDB Shield (AT89S52, 11.0592 MHz) + Oscilloscope Probe (`P1.1`)  
**TASK NO:** 5B.2  

---

### Circuit Diagram
![Circuit Diagram](./FINAL_CAT5B.jpg)

---

### Core Logic

* **1. SFR Inclusion, Output Pin Mapping & Reload Globals**
  Includes `<reg52.h>` for AT89S52 Special Function Register definitions, maps `P1.1` to `OUTPUT` for oscilloscope probing, and defines `reload_high` and `reload_low` (defaulting to `0xFE33` for a `1 kHz` square wave) to dynamically control the Timer 1 half-period.
  ```c
  #include <reg52.h>

  sbit OUTPUT = P1^1;

  unsigned char reload_high = 0xFE;
  unsigned char reload_low  = 0x33;
  ```

* **2. Multi-Mode Frequency Lookup (`set_frequency`)**
  Updates `reload_high` and `reload_low` across five preset operating modes—**Mode 1** (`0xEE00` → `100 Hz`), **Mode 2** (`0xFC66` → `500 Hz`), **Mode 3** (`0xFE33` → `1 kHz`), **Mode 4** (`0xFF1A` → `2 kHz`), and **Mode 5** (`0xFFA4` → `5 kHz`)—matching the half-period tick counts required at `11.0592 MHz` ($T_{\text{machine}} \approx 1.085\ \mu\text{s}$).
  ```c
  void set_frequency(unsigned char mode)
  {
      switch(mode)
      {
          case 1: reload_high = 0xEE; reload_low = 0x00; break; // 100 Hz
          case 2: reload_high = 0xFC; reload_low = 0x66; break; // 500 Hz
          case 3: reload_high = 0xFE; reload_low = 0x33; break; // 1 kHz
          case 4: reload_high = 0xFF; reload_low = 0x1A; break; // 2 kHz
          case 5: reload_high = 0xFF; reload_low = 0xA4; break; // 5 kHz
          default: break;
      }
  }
  ```

* **3. Timer 1 Mode 1 Initialization & Half-Period Pin Toggle ISR**
  Configures Timer 1 in 16-bit Mode 1 (`TMOD = 0x10`), loads the initial `TH1`/`TL1` values, enables `ET1` and `EA`, and starts `TR1`. On every Timer 1 overflow (`interrupt 3`), `timer1_ISR` reloads `TH1` and `TL1` and complements `OUTPUT` (`OUTPUT = ~OUTPUT`), producing a symmetrical `50%` duty-cycle square wave on `P1.1` without blocking the main loop.
  ```c
  void timer1_ISR(void) interrupt 3
  {
      TH1 = reload_high;
      TL1 = reload_low;

      OUTPUT = ~OUTPUT;
  }

  void main(void)
  {
      TMOD = 0x10;
      TH1 = reload_high;
      TL1 = reload_low;

      ET1 = 1;
      EA = 1;
      TR1 = 1;

      while(1)
      {
          /* Call set_frequency(1)..(5) to switch output frequency */
      }
  }
  ```

---

### Inference
Configuring Timer 1 in 16-bit Mode 1 and toggling `P1.1` inside `timer1_ISR` on every half-period overflow generated a clean, symmetrical `50%` duty-cycle square wave across selectable frequencies from `100 Hz` to `5 kHz` on the `11.0592 MHz` AT89S52 board. Updating the `TH1` and `TL1` preload registers dynamically via `set_frequency()` allowed instantaneous frequency scaling on the digital oscilloscope without using blocking delay loops.