# Semaphore Alphabet Trainer (Arduino)

Every so often the Plusscouts PA3EFR/J produce some JOTA related solder projects and/or software design. 

This project is an Arduino-based semaphore alphabet trainer using LEDs and push buttons.  
It supports **manual step-by-step signalling**, **random training**, **numeric signalling**, and an **automatic power-on self test**.

The system is designed for learning, demonstrating, or experimenting with semaphore-style signalling logic.

---

## Features

- **Semaphore alphabet A–Z** using 7 LEDs
- **Two operating modes** selectable by hardware switch
  - Text mode
  - Random mode
- **Numeric signalling support** with explicit start/end markers
- **Startup self-test sequence**
- Fully **step-controlled** using a momentary push button
- Deterministic timing for all special sequences

---

## Hardware Setup

### LEDs
- 7 LEDs connected to Arduino pins **2–8**
- Each LED represents one semaphore arm position

### Inputs
| Pin | Type | Function |
|----:|-----|----------|
| 9 | Toggle switch | Mode select (LOW = Text, HIGH = Random) |
| 10 | Momentary button | Advance to next step |

Internal pull-up resistors are used for all inputs.

---

## Operating Modes

### 1. Startup Test Mode (Automatic)

When the Arduino powers up or resets, a self-test is played **once**:

- Sequence:  
nrudurnabcdefgfedcbadndrdudrdn

- Each letter is shown for **0.35 seconds**
- After the sequence:
- **All LEDs ON** for **0.75 seconds**
- LEDs OFF
- After this, normal operation begins

This test verifies:
- LED wiring
- Alphabet mapping
- Timing consistency

---

### 2. Text Mode (Pin 9 = LOW)

Text mode sends a predefined text **step by step** using the button on pin 10.

#### Text start & end signal
Before and after the text, a notification sequence is sent:

rudurudur


- Total duration: **2 seconds**
- Requires a button press to start

#### Letters
- Each button press sends **one letter**
- Spaces turn **all LEDs off**

#### Numbers in text
Numbers are handled as **groups** and are clearly marked.

**Start digits marker**
{0,0,0,1,1,0,0}


**End digits marker**
{0,0,0,1,0,1,0}


- The start marker is sent **once before the first digit**
- All digits follow sequentially
- The end marker is sent **once after the last digit**

##### Digit mapping
| Digit | Semaphore Letter |
|------:|------------------|
| 0 | K |
| 1 | A |
| 2 | B |
| 3 | C |
| 4 | D |
| 5 | E |
| 6 | F |
| 7 | G |
| 8 | H |
| 9 | I |

Example:
"welcome to JOTA 2026"

Results in:
Start digits → 2 → 0 → 2 → 6 → End digits


---

### 3. Random Mode (Pin 9 = HIGH)

Each button press generates either:
- a random letter **or**
- a random digit

#### Random letters
- Displayed immediately
- One letter per button press

#### Random digits
Each digit is sent as a **self-contained sequence**:

1. Start digits marker — **0.5 s**
2. Digit — **1.0 s**
3. End digits marker — **0.5 s**
4. LEDs OFF, wait for next button press

Digits in random mode are **independent** and never grouped.

---

## Customization

You can easily modify:

### Text content
```cpp
const char text[] = "your text here";
Timing values
Startup test timing
Intro/outro duration
Digit signalling duration
Alphabet mapping
Adjust the letters[][7] table to match your own semaphore layout.

Design Notes
The system uses blocking timing (delay) for clarity and predictability
State machines are used to manage text flow and digit groups
Random and text modes are fully independent
Startup test cannot be interrupted

Possible Extensions
Non-blocking (millis) timing
Multiple selectable texts
Adjustable letter/digit speed
PWM brightness control
External semaphore arms instead of LEDs

License
This project is intended for educational and experimental use.
Use, modify, and share freely.
Have fun, Erwin PA3EFR
