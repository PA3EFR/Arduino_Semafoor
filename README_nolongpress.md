# Semaphore Alphabet Trainer (Arduino)

De Plusscouts PA3EFR/J zijn al jaren bezig met soldeer- en programmeerprojecten. Ook dit project past in het educatieve traject om scouts kennis te laten maken met semafoor seinen. Dit project is een Arduino‑gebaseerde **semafoor‑alfabet trainer** met LEDs en een drukknop. Het systeem is bedoeld voor leren, demonstreren en experimenteren met semafoorsignalen, en bevat uitgebreide logica voor **letters, cijfers, automatische testmodi en idle‑gedrag**.

Deze README is bijgewerkt zodat hij overeenkomt met de **complete eindversie** van `nieuw_script.txt`.

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

| Pin | Type             | Functie                                    |
| --: | ---------------- | ------------------------------------------ |
|  12 | Tuimelschakelaar | Modusselectie (LOW = Tekst, HIGH = Random) |
|  10 | Drukknop         | Volgende stap                              |

Alle ingangen gebruiken **interne pull‑up weerstanden**.

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
const char textMessage[] = "pa3efr ";
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
  * Normale werking hervat

Ideaal voor:

* Demonstraties
* Tentoonstellingen
* Onbemand gebruik

---

## Instelbare parameters

Belangrijke aanpasbare instellingen:

```cpp
const unsigned long debounceTime = 50;
const unsigned long idleTimeout  = 15000;
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
* Willekeur‑balans
* Alfabet‑mapping (`letters[][7]`)

---

## Ontwerpnotities

* Edge‑based knop‑afhandeling met debounce
* Startup‑ en idle‑tests delen dezelfde engine
* Gebruikersinput heeft altijd prioriteit
* Geen cognitieve herhaling in randommodus
* Blokkerende delays alleen binnen duidelijke signaalpatronen

---

## Licentie

Vrij te gebruiken voor educatieve en experimentele doeleinden.

Veel plezier,
**Erwin PA3EFR**
