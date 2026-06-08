# Semaphore Alphabet Trainer (Arduino)

De Plusscouts PA3EFR/J zijn al jaren bezig met soldeer- en programmeerprojecten. Ook dit project past in het educatieve traject om scouts kennis te laten maken met semafoor seinen. Dit project is een Arduino‑gebaseerde **semafoor‑alfabet trainer** met LEDs en een drukknop. Het systeem is bedoeld voor leren, demonstreren en experimenteren met semafoorsignalen, en bevat uitgebreide logica voor **letters, cijfers, automatische testmodi, idle‑gedrag en een verborgen boodschap**.

Deze README is bijgewerkt zodat hij overeenkomt met de **uitgebreide versie** van `semaphore_trainer.ino` (inclusief long-press functie voor de verborgen IDLE-message).

---

## Features

* **Semaphore alfabet A–Z** met 7 LEDs
* **Letters én cijfers** ondersteund in zowel tekst‑ als randommodus
* **Twee hoofdmodi**, geselecteerd via een hardware‑schakelaar

  * Tekstmodus (vooraf ingestelde tekst)
  * Randommodus (gewogen willekeur)
* **Cijferondersteuning met officiële start‑ en eindmarkering**
* **Automatische startup self‑test** bij inschakelen
* **Uitgebreide idle / attract‑modus** met meerdere fases
* **Verborgen IDLE-message via long-press**

  * Door de knop langer dan **3 seconden** ingedrukt te houden, wordt de
    verborgen boodschap `"PA3EFR ECHT WEL GAAF"` stapsgewijs doorlopen
  * Het tempo waarmee de boodschap wordt afgespeeld wordt bepaald door
    **hoe snel de gebruiker de knop indrukt**
* **Robuuste knop‑debouncing**
* **Geen herhaling van hetzelfde symbool** (letter of cijfer)
* **Cijfer‑cooldown**: na een cijfer volgen altijd meerdere letters
* Volledig **stap‑voor‑stap** bediend met één drukknop

---

## Hardware‑opstelling

### LEDs

* 7 LEDs aangesloten op Arduino‑pinnen:

```
{3, 5, 7, 8, 6, 4, 2}
```

* Elke LED stelt één semafoorarm‑positie voor

### Ingangen

| Pin | Type             | Functie                                                    |
| --: | ---------------- | ---------------------------------------------------------- |
|  12 | Tuimelschakelaar | Modusselectie (LOW = Tekst, HIGH = Random)                 |
|  10 | Drukknop         | Volgende stap **of** long-press → verborgen message        |

Alle ingangen gebruiken **interne pull‑up weerstanden**.

De drukknop heeft **twee gedragingen**, afhankelijk van hoe lang hij wordt
ingedrukt:

* **Korte druk** (< 3 s): normale stap binnen de gekozen modus
* **Lange druk** (≥ 3 s): activeert de verborgen IDLE-message cyclus
  (zie sectie 5 hieronder)

---

## Modusoverzicht

### 1. Startup Test (automatisch)

Bij het opstarten wordt **eenmalig** een zelftest afgespeeld.

* Testsequentie:

```
abcdefgfedcbanrudurngfedcbabcdefgnrudurn
```

* Elke stap wordt ongeveer **80 ms** weergegeven
* De test is **niet onderbreekbaar**
* Na afloop begint de normale werking

Doel van deze test:

* Controleren van LED‑bedrading
* Controleren van alfabet‑mapping
* Visuele feedback bij inschakelen

---

### 2. Tekstmodus (schakelaar = LOW)

In tekstmodus wordt een vooraf ingestelde tekst **letter voor letter** weergegeven.

```cpp
const char textMessage[] = "pa3efr";
```

* Elke druk op de knop toont **één teken**
* Spaties schakelen **alle LEDs uit** (pauze)
* Na het einde van de tekst begint deze opnieuw

#### Cijfers in tekstmodus

Cijfers worden altijd als een **volledig cijferpatroon** verzonden:

1. **Start‑cijfermarker** (1,5 s)
2. Het cijfer zelf (2,0 s)
3. **Eind‑cijfermarker** (1,5 s)

Start‑ en eindmarkeringen:

```cpp
startDigits = {0,0,0,1,1,0,0}
endDigits   = {0,0,0,1,0,1,0}
```

Cijfers worden vertaald naar letters volgens:

| Cijfer | Letter |
| -----: | ------ |
|      0 | K      |
|      1 | A      |
|      2 | B      |
|      3 | C      |
|      4 | D      |
|      5 | E      |
|      6 | F      |
|      7 | G      |
|      8 | H      |
|      9 | I      |

---

### 3. Randommodus (schakelaar = HIGH)

Elke druk op de knop genereert **één nieuw symbool**:

* Een letter **of**
* Een cijfer (met volledig cijferpatroon)

#### Gewogen willekeur

```cpp
randomLettersWeight = 20;
randomDigitsWeight  = 2;
```

* Letters verschijnen veel vaker dan cijfers

#### Cijfer‑regels

* Na elk cijfer volgen **minstens 5 letters**
* Twee cijfers kunnen nooit kort na elkaar verschijnen
* Hetzelfde cijfer of dezelfde letter wordt nooit herhaald

Cijfers in randommodus worden altijd verzonden als:

```
startDigits → cijfer → endDigits
```

---

### 4. Idle / Attract‑modus (automatisch)

Wanneer de knop **15 seconden niet wordt ingedrukt**, gaat het systeem automatisch in idle‑modus.

Idle‑modus bestaat uit **drie fases**:

1. **Testsequentie** (10 seconden)
   * Speelt dezelfde testsequentie als bij het opstarten
   * Elke letter wordt 80 ms weergegeven

2. **Snelle tekst "PA3EFR ECHT WEL GAAF"** (2 seconden)
   * Toont letters in vaste volgorde: **P‑A‑3‑E‑F‑R‑E‑C‑H‑T‑W‑E‑L‑G‑A‑A‑F**
   * Elke stap wordt slechts 10 ms weergegeven (versneld)
   * Het cijfer 3 wordt weergegeven als een volledig cijferpatroon:
     `startDigits → 3 → endDigits` (150 ms totaal)
   * Spaties tussen woorden worden overgeslagen
   * Na afloop herhaalt de tekst automatisch

3. **Vaststaande letter** (5 seconden)
   * Toont één willekeurige letter
   * Blijft 5 seconden staan

Na **7 cycli** van fase 2 en 3 wordt de idle‑modus gereset en begint weer bij fase 1.

#### Visualisatie idle‑modus

```
Fase 1: Testsequentie (10s)  →  abcdefgfedcba...
         ↓
Fase 2: "PA3EFR ECHT WEL GAAF" (2s)  →  P-A-3-E-F-R-E-C-H-T-W-E-L-G-A-A-F
         ↓
Fase 3: Vaste letter (5s)  →  [willekeurige letter]
         ↓
    (herhaal fase 2-3, max 7x, dan terug naar fase 1)
```

* Zodra de knop wordt ingedrukt:

  * Idle stopt onmiddellijk
  * De long-press state wordt gereset
  * Normale werking hervat

Ideaal voor:

* Demonstraties
* Tentoonstellingen
* Onbemand gebruik

---

### 5. Long-press: Verborgen IDLE-message

Naast de gewone korte‑druk functie heeft de drukknop een **tweede functie** die
wordt geactiveerd door de knop **langer dan 3 seconden** ingedrukt te houden.

#### Wat gebeurt er?

* Bij het bereiken van de 3‑secondendrempel wordt de **verborgen boodschap**

  ```
  PA3EFR ECHT WEL GAAF
  ```

  op de LEDs getoond.
* Elk **nieuwe indrukken** van de knop (loslaten + opnieuw indrukken) zorgt
  ervoor dat de boodschap **één teken verder** gaat.
* Het tempo van de boodschap wordt dus bepaald door **hoe snel (of langzaam)
  de gebruiker drukt**: snel tikken = snel door de boodschap, rustig tikken
  = elk teken goed kunnen aflezen.
* Aan het einde van de boodschap wordt teruggewrapt naar het begin.

#### Weergave per teken

| Teken          | Weergave                            |
| -------------- | ----------------------------------- |
| A‑Z            | Bijbehorende letter‑positie         |
| 0‑9            | Bijbehorende letter (via `digitMap`)|
| spatie         | Alle LEDs uit (pauze tussen woorden)|

#### Cyclus starten en verlaten

| Actie                                | Effect                                  |
| ------------------------------------ | --------------------------------------- |
| Knop ≥ 3 s vasthouden                | Cyclus start, eerste teken verschijnt   |
| Loslaten                             | LEDs blijven staan op huidige teken     |
| Opnieuw indrukken                    | Volgende teken van de boodschap         |
| Knop ≥ 3 s vasthouden tijdens cyclus | Geen nieuwe actie (cyclus loopt al)     |
| Mode-switch (pin 12) bedienen        | Cyclus wordt netjes verlaten            |
| 15 s geen activiteit → idle          | Cyclus wordt gereset bij idle-start     |

#### Visualisatie

```
   Hou de knop vast
          │
          ▼
   ┌──────────────────────┐
   │ tijd < 3 s?          │── ja ──►  Loslaten = randomStep() / textStep()
   └──────────────────────┘
          │ nee
          ▼
   ┌──────────────────────┐
   │ Cyclus start         │
   │ toon idleFastSeq[0]  │
   └──────────────────────┘
          │
          ▼
   ┌──────────────────────┐         ┌──────────────────────┐
   │ Knop loslaten        │ ──────► │ LEDs blijven staan   │
   └──────────────────────┘         └──────────────────────┘
          │
          ▼
   ┌──────────────────────┐
   │ Opnieuw indrukken    │ ──────► │ toon idleFastSeq[i+1]│
   └──────────────────────┘         └──────────────────────┘
          │
          ▼
   (herhaal tot einde → wrap naar begin)
```

#### Doel

* Leuke **easter egg** / verborgen functie voor wie de trainer lang genoeg
  bestudeert
* Handig om de verborgen boodschap in **eigen tempo** te kunnen ontcijferen
* Maakt duidelijk dat de trainer **meer** kan dan de standaard tekst of random

---

## Instelbare parameters

Belangrijke aanpasbare instellingen:

```cpp
const unsigned long debounceTime   = 50;
const unsigned long idleTimeout    = 15000;
const unsigned long longPressTime  = 3000;   // drempel voor long-press
```

```cpp
const int randomLettersWeight = 20;
const int randomDigitsWeight  = 2;
```

```cpp
int lettersUntilNextDigit = 5;
```

```cpp
const char idleFastSequence[] = "PA3EFR ECHT WEL GAAF";
```

* Tekstinhoud (`textMessage`)
* Idle‑sneltekst (`idleFastSequence`)
* Idle‑gedrag
* Long-press drempel (`longPressTime`, in milliseconden)
* Willekeur‑balans
* Alfabet‑mapping (`letters[][7]`)

---

## Ontwerpnotities

* Edge‑based knop‑afhandeling met debounce
* Startup‑ en idle‑tests delen dezelfde engine
* Gebruikersinput heeft altijd prioriteit
* Geen cognitieve herhaling in randommodus
* Blokkerende delays alleen binnen duidelijke signaalpatronen
* Long-press cyclus deelt de `idleFastSequence` met de idle-attract loop
  (zelfde bron, andere aansturing)
* De long-press state wordt automatisch gereset bij:

  * Mode-switch overgang
  * Idle-timeout

  zodat de trainer nooit "blijft hangen" in een verborgen toestand

---

## Licentie

Vrij te gebruiken voor educatieve en experimentele doeleinden.

Veel plezier,
**Erwin PA3EFR**
