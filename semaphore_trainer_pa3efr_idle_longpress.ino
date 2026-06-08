// ======================================================
// Semaphore Alphabet Trainer – UITGEBREIDE VERSIE
//   • Korte druk op de knop   = normale stap (TEXT of RANDOM)
//   • Lange druk (> 3 sec)    = verborgen IDLE-message
//                              ("PA3EFR ECHT WEL GAAF") wordt
//                              stapsgewijs doorlopen op het
//                              tempo van de drukknop.
//   • Elke nieuwe druk        = volgend teken van de message.
//   • Wordt de mode-switch
//     bediend of het idle-
//     timeout bereikt, dan
//     verlaat het script de
//     long-press cyclus.
// ======================================================

// ---------------- PINNEN ----------------
const int ledPins[7] = {3, 5, 7, 8, 6, 4, 2};
const int modeSwitchPin = 12;   // LOW = TEKST, HIGH = RANDOM
const int nextButtonPin = 10;

// ---------------- TIMING ----------------
const unsigned long debounceTime   = 50;
const unsigned long idleTimeout   = 15000;
const unsigned long longPressTime = 3000;   // drempel voor long-press

// ---------------- RANDOM BALANS ----------------
const int randomLettersWeight = 20;
const int randomDigitsWeight  = 2;

// ---------------- TEKST ----------------
const char textMessage[] = "pa3efr";
int textIndex = 0;

// MESSAGE sequence "rudurudur"
const char messageSeq[] = "rudurudur ";
bool messagePlayedAtStart = false; // is de start-MESSAGE al geweest?

// ---------------- CIJFER COOLDOWN ----------------
int lettersUntilNextDigit = 0;

// ---------------- LAATSTE TEKEN ----------------
bool lastWasDigit = false;
int lastSymbolIndex = -1;

// ---------------- LETTERS ----------------
const bool letters[][7] = {
  {1,0,0,0,0,0,0},{0,1,0,0,0,0,0},{0,0,1,0,0,0,0},
  {0,0,0,1,0,0,0},{0,0,0,0,1,0,0},{0,0,0,0,0,1,0},
  {0,0,0,0,0,0,1},{1,1,0,0,0,0,0},{1,0,1,0,0,0,0},
  {0,0,0,1,0,1,0},{1,0,0,1,0,0,0},{1,0,0,0,1,0,0},
  {1,0,0,0,0,1,0},{1,0,0,0,0,0,1},{0,1,1,0,0,0,0},
  {0,1,0,1,0,0,0},{0,1,0,0,1,0,0},{0,1,0,0,0,1,0},
  {0,1,0,0,0,0,1},{0,0,1,1,0,0,0},{0,0,1,0,1,0,0},
  {0,0,0,1,0,0,1},{0,0,0,0,1,1,0},{0,0,0,0,1,0,1},
  {0,0,1,0,0,1,0},{0,0,0,0,0,1,1}
};
const int numLetters = 26;

// ---------------- CIJFERS ----------------
const bool startDigits[7] = {0,0,0,1,1,0,0};
const bool endDigits[7]   = {0,0,0,1,0,1,0};
const int digitMap[10]    = {10,0,1,2,3,4,5,6,7,8};

// ---------------- TEST SEQUENCE ----------------
const char testSeq[] = "abcdefgfedcbanrudurngfedcbabcdefgnrudurn";

// ---------------- STARTUP TEST ----------------
bool startupTestActive = true;
int startupIndex = 0;
unsigned long startupLastStep = 0;

// ---------------- KNOP ----------------
bool lastButtonState = HIGH;
unsigned long lastButtonChange = 0;
bool buttonIsDown = false;
unsigned long buttonDownTime = 0;
bool longPressTriggered = false;   // is de 3s-drempel bereikt?
bool longPressMode = false;        // zijn we aan het doorlopen?
int longPressIndex = 0;            // positie in idleFastSequence

// ---------------- IDLE ----------------
unsigned long lastUserActionTime = 0;
bool idleActive = false;

enum IdleState {
  IDLE_TESTSEQ,
  IDLE_RANDOM_FAST,
  IDLE_SINGLE_LETTER
};

IdleState idleState = IDLE_TESTSEQ;
unsigned long idleStateStart = 0;
unsigned long idleLastStep = 0;
int idleIndex = 0;
int idleCycleCount = 0;
int fixedIdleLetter = 0;

// ---------------- IDLE FAST SEQUENCE (PA3EFR ECHT WEL GAAF) ----------------
// Wordt ook gebruikt voor de long-press "verborgen message" functie.
const char idleFastSequence[] = "PA3EFR ECHT WEL GAAF";
int idleFastIndex = 0;

// ---------------- MODE TRACKING ----------------
int lastModeState = HIGH;

// ======================================================
// HULP: MESSAGE automatisch spelen (ca. 2 s totaal)
// ======================================================
void playMessage() {
  for (int i = 0; messageSeq[i] != '\0'; i++) {
    char c = messageSeq[i];
    if (c >= 'a' && c <= 'z') {
      showLetter(c - 'a');
      delay(200);
    }
  }
}

// ======================================================
// ALLE LEDS UIT (voor state transitions)
// ======================================================
void allLEDsOff() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

// ======================================================
// SHOW DIGIT (voor idle fast mode - snelle versie)
// ======================================================
void showDigitFast(int d) {
  showPattern(startDigits);
  delay(50);
  showLetter(digitMap[d]);
  delay(50);
  showPattern(endDigits);
  delay(50);
}

// ======================================================
// Toon één teken van de verborgen IDLE-message.
// Wordt gebruikt door de long-press functie.
// Letters worden direct getoond;
// cijfers als bijbehorende letter (digitMap);
// spatie = LEDS uit.
// ======================================================
void showLongPressChar(int index) {
  char c = idleFastSequence[index];
  if (c == ' ') {
    allLEDsOff();
  } else if (c >= 'A' && c <= 'Z') {
    showLetter(c - 'A');
  } else if (c >= 'a' && c <= 'z') {
    showLetter(c - 'a');
  } else if (c >= '0' && c <= '9') {
    int d = c - '0';
    showLetter(digitMap[d]);
  } else {
    allLEDsOff();
  }
}

// ======================================================
// SETUP
// ======================================================
void setup() {
  for (int i = 0; i < 7; i++) pinMode(ledPins[i], OUTPUT);
  pinMode(modeSwitchPin, INPUT_PULLUP);
  pinMode(nextButtonPin, INPUT_PULLUP);

  randomSeed(analogRead(A0));

  startupTestActive = true;
  startupIndex = 0;
  startupLastStep = 0;

  // Knop-state
  lastButtonState = HIGH;
  lastButtonChange = 0;
  buttonIsDown = false;
  buttonDownTime = 0;
  longPressTriggered = false;
  longPressMode = false;
  longPressIndex = 0;

  lastModeState = digitalRead(modeSwitchPin);
}

// ======================================================
// LOOP
// ======================================================
void loop() {

  // --- Detectie overgang RANDOM -> TEKST en tekst-modus resetten ---
  int currentModeState = digitalRead(modeSwitchPin);
  if (lastModeState == HIGH && currentModeState == LOW) {
    textIndex = 0;
    messagePlayedAtStart = false;
    // Mode-switch verlaten eventueel de long-press cyclus
    longPressMode = false;
    longPressTriggered = false;
    buttonIsDown = false;
  }
  lastModeState = currentModeState;

  if (startupTestActive) {
    handleStartupTest();
    return;
  }

  handleNextButton();
  handleIdle();
}

// ======================================================
// STARTUP TEST
// ======================================================
void handleStartupTest() {
  unsigned long now = millis();

  if (now - startupLastStep < 80) return;
  startupLastStep = now;

  char c = testSeq[startupIndex++];

  if (c == '\0') {
    startupTestActive = false;
    lastUserActionTime = millis();
    return;
  }

  showLetter(c - 'a');
}

// ======================================================
// KNOP
//   • Korte druk           -> randomStep() / textStep()
//   • Hold > 3 s           -> long-press cyclus start
//   • Tijdens cyclus       -> elke nieuwe druk = volgend teken
//   • Loslaten             -> cyclus blijft actief (LEDs blijven)
//                             tot idle of mode-switch
// ======================================================
void handleNextButton() {
  bool reading = digitalRead(nextButtonPin);
  unsigned long now = millis();

  if (reading != lastButtonState) {
    lastButtonChange = now;
    lastButtonState = reading;
  }

  if (now - lastButtonChange <= debounceTime) return;

  // ---- FALLING EDGE: knop net ingedrukt ----
  if (reading == LOW && !buttonIsDown) {
    buttonIsDown = true;
    buttonDownTime = now;
    longPressTriggered = false;
    idleActive = false;
    lastUserActionTime = now;

    if (longPressMode) {
      // We lopen al door de verborgen message: naar volgend teken
      longPressIndex++;
      if (idleFastSequence[longPressIndex] == '\0') longPressIndex = 0;
      showLongPressChar(longPressIndex);
    }
    return;
  }

  // ---- RISING EDGE: knop net losgelaten ----
  if (reading == HIGH && buttonIsDown) {
    buttonIsDown = false;

    if (!longPressMode && !longPressTriggered) {
      // Korte druk = normale stap
      if (digitalRead(modeSwitchPin) == HIGH) {
        randomStep();
      } else {
        textStep();
      }
    }
    // Bij long-press: LEDs blijven staan, mode blijft actief
    return;
  }

  // ---- KNOP NOG INGEDRUKT: check of 3s-drempel bereikt is ----
  if (reading == LOW && buttonIsDown && !longPressTriggered) {
    if (now - buttonDownTime >= longPressTime) {
      // Long-press geactiveerd: start de verborgen message cyclus
      longPressTriggered = true;
      longPressMode = true;
      longPressIndex = 0;
      showLongPressChar(longPressIndex);
      lastUserActionTime = now;
    }
  }
}

// ======================================================
// IDLE (met glitch-preventie)
// ======================================================
void handleIdle() {

  if (!idleActive) {
    if (millis() - lastUserActionTime > idleTimeout) {
      idleActive = true;
      // Bij idle de long-press cyclus veilig verlaten
      longPressMode = false;
      longPressTriggered = false;
      buttonIsDown = false;

      idleState = IDLE_TESTSEQ;
      idleStateStart = millis();
      idleLastStep = 0;
      idleIndex = 0;
      idleCycleCount = 0;
    }
    return;
  }

  unsigned long now = millis();

  switch (idleState) {

    case IDLE_TESTSEQ:
      if (now - idleLastStep >= 80) {
        idleLastStep = now;
        char c = testSeq[idleIndex++];
        if (c == '\0') idleIndex = 0;
        showLetter(c - 'a');
      }
      if (now - idleStateStart >= 10000) {
        // GLITCH FIX: alle LEDs uit voor state change
        allLEDsOff();
        delay(10);

        idleState = IDLE_RANDOM_FAST;
        idleStateStart = now;
        idleLastStep = now;  // voorkomt instant showLetter()
        idleCycleCount = 0;
        idleFastIndex = 0;   // reset sequence index
      }
      break;

    case IDLE_RANDOM_FAST:
      if (now - idleLastStep >= 10) {
        idleLastStep = now;

        // Haal huidig teken uit sequence
        char c = idleFastSequence[idleFastIndex];

        // Wrap around aan het einde
        if (c == '\0') {
          idleFastIndex = 0;
          c = idleFastSequence[idleFastIndex];
        }

        // Handle speciale tekens
        if (c == ' ') {
          // Spaties overslaan (LEDs kort uit)
          allLEDsOff();
          idleFastIndex++;
        } else if (c >= 'A' && c <= 'Z') {
          // Letters weergeven
          showLetter(c - 'A');
          idleFastIndex++;
        } else if (c >= 'a' && c <= 'z') {
          // Kleine letters ook ondersteunen
          showLetter(c - 'a');
          idleFastIndex++;
        } else if (c >= '0' && c <= '9') {
          // Cijfers weergeven met cijfer-pattern
          int d = c - '0';
          showDigitFast(d);
          idleFastIndex++;
        } else {
          // Onbekend teken overslaan
          idleFastIndex++;
        }
      }
      if (now - idleStateStart >= 2000) {
        // GLITCH FIX: alle LEDs uit voor state change
        allLEDsOff();
        delay(10);

        fixedIdleLetter = random(numLetters);
        showLetter(fixedIdleLetter);
        idleState = IDLE_SINGLE_LETTER;
        idleStateStart = now;
      }
      break;

    case IDLE_SINGLE_LETTER:
      if (now - idleStateStart >= 5000) {
        idleCycleCount++;
        if (idleCycleCount >= 7) {
          // GLITCH FIX: alle LEDs uit voor state change
          allLEDsOff();
          delay(10);

          idleState = IDLE_TESTSEQ;
          idleStateStart = now;
          idleIndex = 0;
        } else {
          // GLITCH FIX: alle LEDs uit voor state change
          allLEDsOff();
          delay(10);

          idleState = IDLE_RANDOM_FAST;
          idleStateStart = now;
          idleLastStep = now;
          idleFastIndex = 0;  // reset sequence index
        }
      }
      break;
  }
}

// ======================================================
// TEKST MODUS (LETTERS + CIJFERS + MESSAGE)
// ======================================================
void textStep() {

  if (!messagePlayedAtStart) {
    playMessage();
    messagePlayedAtStart = true;
    textIndex = 0;
  }

  char c = textMessage[textIndex++];

  if (c == '\0') {
    textIndex = 0;
    messagePlayedAtStart = false;
    playMessage();
    allLEDsOff();
    return;
  }

  if (c == ' ') {
    allLEDsOff();
    return;
  }

  if (c >= 'a' && c <= 'z') {
    showLetter(c - 'a');
    return;
  }

  if (c >= '0' && c <= '9') {
    int d = c - '0';
    showPattern(startDigits);
    delay(1500);
    showLetter(digitMap[d]);
    delay(2000);
    showPattern(endDigits);
    delay(1500);
  }
}

// ======================================================
// RANDOM MODUS (LETTERS + CIJFERS)
// ======================================================
void randomStep() {

  if (lettersUntilNextDigit > 0) {
    int l;
    do { l = random(numLetters); }
    while (!lastWasDigit && l == lastSymbolIndex);
    showLetter(l);
    lastWasDigit = false;
    lastSymbolIndex = l;
    lettersUntilNextDigit--;
    return;
  }

  int total = randomLettersWeight + randomDigitsWeight;

  if (random(total) < randomDigitsWeight) {
    int d;
    do { d = random(10); }
    while (lastWasDigit && d == lastSymbolIndex);
    playRandomDigit(d);
    lastWasDigit = true;
    lastSymbolIndex = d;
    lettersUntilNextDigit = 5;
  } else {
    int l;
    do { l = random(numLetters); }
    while (!lastWasDigit && l == lastSymbolIndex);
    showLetter(l);
    lastWasDigit = false;
    lastSymbolIndex = l;
  }
}

void playRandomDigit(int d) {
  showPattern(startDigits);
  delay(1500);
  showLetter(digitMap[d]);
  delay(2000);
  showPattern(endDigits);
  delay(1500);
}

// ======================================================
// HULPFUNCTIES
// ======================================================
void showLetter(int i) {
  for (int l = 0; l < 7; l++)
    digitalWrite(ledPins[l], letters[i][l]);
}

void showPattern(const bool p[7]) {
  for (int i = 0; i < 7; i++)
    digitalWrite(ledPins[i], p[i]);
}
