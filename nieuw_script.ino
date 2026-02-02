// ======================================================
// Semaphore Alphabet Trainer – COMPLETE EINDVERSIE
// ======================================================

// ---------------- PINNEN ----------------
const int ledPins[7] = {3, 5, 7, 8, 6, 4, 2};
const int modeSwitchPin = 12;   // LOW = TEKST, HIGH = RANDOM
const int nextButtonPin = 10;

// ---------------- TIMING ----------------
const unsigned long debounceTime = 50;
const unsigned long idleTimeout  = 15000;

// ---------------- RANDOM BALANS ----------------
const int randomLettersWeight = 20;
const int randomDigitsWeight  = 2;

// ---------------- TEKST ----------------
const char textMessage[] = "pa3efr ";
int textIndex = 0;

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
bool buttonHandled = false;
unsigned long lastButtonChange = 0;

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
}

// ======================================================
// LOOP
// ======================================================
void loop() {

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
// ======================================================
void handleNextButton() {
  bool reading = digitalRead(nextButtonPin);
  unsigned long now = millis();

  if (reading != lastButtonState) {
    lastButtonChange = now;
    lastButtonState = reading;
  }

  if (now - lastButtonChange > debounceTime) {
    if (reading == LOW && !buttonHandled) {
      buttonHandled = true;
      idleActive = false;
      lastUserActionTime = now;

      if (digitalRead(modeSwitchPin) == HIGH) {
        // RANDOM modus: nu met letters + cijfers, maar startDigits alleen in cijferpatroon
        randomStep();
      } else {
        // TEKST modus
        textStep();
      }
    }
    if (reading == HIGH) buttonHandled = false;
  }
}

// ======================================================
// IDLE
// ======================================================
void handleIdle() {

  if (!idleActive) {
    if (millis() - lastUserActionTime > idleTimeout) {
      idleActive = true;
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
        idleState = IDLE_RANDOM_FAST;
        idleStateStart = now;
        idleLastStep = 0;
        idleCycleCount = 0;
      }
      break;

    case IDLE_RANDOM_FAST:
      if (now - idleLastStep >= 10) {
        idleLastStep = now;
        // In idle alleen letters, geen startDigits of cijfers
        showLetter(random(numLetters));
      }
      if (now - idleStateStart >= 2000) {
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
          idleState = IDLE_TESTSEQ;
          idleStateStart = now;
          idleIndex = 0;
        } else {
          idleState = IDLE_RANDOM_FAST;
          idleStateStart = now;
          idleLastStep = 0;
        }
      }
      break;
  }
}

// ======================================================
// TEKST MODUS (LETTERS + CIJFERS)
// ======================================================
void textStep() {
  char c = textMessage[textIndex++];

  if (c == '\0') {
    textIndex = 0;
    c = textMessage[textIndex++];
  }

  // spatie = pauze
  if (c == ' ') {
    for (int i = 0; i < 7; i++)
      digitalWrite(ledPins[i], LOW);
    return;
  }

  // letter
  if (c >= 'a' && c <= 'z') {
    showLetter(c - 'a');
    return;
  }

  // cijfer (startDigits en endDigits alleen hier en bij randomStep/playRandomDigit)
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
// RANDOM MODUS (LETTERS + CIJFERS, correct cijferpatroon)
// ======================================================
void randomStep() {

  // Als we nog letters moeten tonen vóór een volgend cijfer
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
    // Cijfer: altijd via startDigits -> digit -> endDigits
    int d;
    do { d = random(10); }
    while (lastWasDigit && d == lastSymbolIndex);

    playRandomDigit(d);
    lastWasDigit = true;
    lastSymbolIndex = d;
    lettersUntilNextDigit = 5;
  } else {
    // Letter
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