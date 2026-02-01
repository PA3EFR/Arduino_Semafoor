# Semaphore Alphabet Trainer (Arduino)

Every so often the Plusscouts PA3EFR/J produce some JOTA related solder projects and/or software design.

This project is an Arduino-based semaphore alphabet trainer using LEDs and push buttons.
It supports **manual step-by-step signalling**, **random training**, **numeric signalling**, an **automatic power-on self test**, and an **idle attract/test mode**.

The system is designed for learning, demonstrating, or experimenting with semaphore-style signalling logic, with special care taken to make the interaction robust and user-friendly even with imperfect hardware.

---

## Features

* **Semaphore alphabet A–Z** using 7 LEDs
* **Two operating modes** selectable by hardware switch

  * Text mode
  * Random mode
* **Numeric signalling support** with explicit start/end markers
* **Startup self-test sequence** (runs once after power-up)
* **Idle test / attract mode** after inactivity
* **Robust button debouncing** (suitable for older or noisy switches)
* **Weighted random generator** (letters appear much more often than digits)
* **Digit spacing logic**: digits are always separated by multiple letters
* **No repeated symbols**: the same letter or digit is never shown twice in a row
* Fully **step-controlled** using a momentary push button

---

## Hardware Setup

### LEDs

* 7 LEDs connected to Arduino pins **2–8**
* Each LED represents one semaphore arm position

### Inputs

| Pin | Type             | Function                                |
| --: | ---------------- | --------------------------------------- |
|  13 | Toggle switch    | Mode select (LOW = Text, HIGH = Random) |
|  10 | Momentary button | Advance to next step                    |

Internal pull-up resistors are used for all inputs.

---

## Operating Modes

### 1. Startup Test Mode (Automatic)

When the Arduino powers up or resets, a self-test is played **once**:

* Sequence:
  `nrudurnabcdefgfedcbadndrdudrdn`

* Each letter is shown for **0.35 seconds**

* After the sequence:

  * LEDs are cleared

* After this, normal operation begins

This test verifies:

* LED wiring
* Alphabet mapping
* Timing consistency

The startup test **cannot be interrupted**.

---

### 2. Text Mode (Mode switch = LOW)

Text mode sends a predefined text **step by step** using the button on pin 10.

#### Text start & end signal

Before and after the text, a notification sequence is sent:

```
rudurudur
```

* Total duration: **2 seconds**
* Requires a button press to start

#### Letters

* Each button press sends **one letter**
* Spaces turn **all LEDs off**

#### Numbers in text

Numbers are handled as **groups** and are clearly marked.

**Start digits marker**

```
{0,0,0,1,1,0,0}
```

**End digits marker**

```
{0,0,0,1,0,1,0}
```

* The start marker is sent **once before the first digit**
* All digits follow sequentially
* The end marker is sent **once after the last digit**

##### Digit mapping

| Digit | Semaphore Letter |
| ----: | ---------------- |
|     0 | K                |
|     1 | A                |
|     2 | B                |
|     3 | C                |
|     4 | D                |
|     5 | E                |
|     6 | F                |
|     7 | G                |
|     8 | H                |
|     9 | I                |

---

### 3. Random Mode (Mode switch = HIGH)

Each button press generates **one unique symbol**:

* a random letter **or**
* a random digit

#### Random balance

* Letters and digits are chosen using **weighted randomness**
* Default ratio: **20 letters : 2 digits**
* This makes letters appear far more often than digits

#### Digit spacing rule

* After any digit, **at least 5 letters** are always generated
* Two digits can never appear close together

#### No repetition rule

* The same letter is never shown twice in a row
* The same digit is never shown twice in a row
* This rule applies across letter ↔ digit transitions

#### Random digits

Each digit is sent as a **self-contained sequence**:

1. Start digits marker — **1.5 s**
2. Digit — **2.0 s**
3. End digits marker — **1.5 s**
4. LEDs off, wait for next button press

Digits in random mode are **never grouped**.

---

### 4. Idle Test / Attract Mode (Automatic)

If the step button is **not pressed for 15 seconds**:

* The system automatically enters **test / attract mode**
* The same test sequence as the startup test is played
* The sequence **repeats continuously**

As soon as the step button is pressed:

* The idle test stops immediately
* Normal operation resumes

This makes the device suitable for:

* demonstrations
* exhibitions
* unattended displays

---

## Customization

You can easily modify:

* **Text content**

```cpp
const char text[] = "your text here";
```

* **Random balance (letters vs digits)**

```cpp
const int randomLettersWeight = 20;
const int randomDigitsWeight  = 2;
```

* **Minimum letters after a digit**

```cpp
lettersUntilNextDigit = 5;
```

* **Idle timeout**

```cpp
const unsigned long idleTimeout = 15000;
```

* **Debounce time for the step button**

```cpp
const unsigned long debounceTime = 50;
```

* **Alphabet mapping**
  Adjust the `letters[][7]` table to match your own semaphore layout.

---

## Design Notes

* Button handling uses **robust edge-based debouncing**
* User input always has priority over automatic behaviour
* Startup test and idle test share the same engine
* Random generation avoids cognitive repetition
* Blocking delays are only used inside clearly defined signalling sequences

---

## Possible Extensions

* Long-press or auto-repeat on the step button
* Different idle sequences
* Multiple selectable texts
* Adjustable speed profiles
* PWM brightness control
* External semaphore arms instead of LEDs

---

## License

This project is intended for educational and experimental use.

Use, modify, and share freely.

Have fun,
**Erwin PA3EFR**
