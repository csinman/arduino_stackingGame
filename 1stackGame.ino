/* 
LED Stacker Game with Piezo Melody
Author: Ceren Sinman
Date 2026-01-12
Purpose: (4x3) LED stacker timing game on Arduino UNO.
        - User presses a button to stop moving LED on the middle LED of each row.
        - Buzzer plays a short beep on the correct hit.
        - A melody plays when all stages are completed.
        After winning, pressing the button restarts the game.
*/

// LED STACKER GAME WITH WIN MELODY + RESTART

// Step 1: Pin defintions

const int buttonPin = A0;
const int buzzerPin = A1;

const int leds[] = {
  2, 3, 4,      // stage 0
  5, 6, 7,      // stage 1
  8, 9, 10,     // stage 2
  11, 12, 13    // stage 3
};

// Step 2: Game state variables

bool locked[12] = {false};
int stage = 0;
int indexPos = 0;
unsigned long lastMove = 0;
const int speedMs = 150;

bool lastButton = LOW;
bool gameWon = false;

// Step 3: Win melody data

int songLength = 17;
char notes[] = "cdaf gf adfgc gc ";
int beats[] = {1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,3}; // "1" represents a quarter-note, 2 a half-note
int tempo = 130; // how fast to play the song

// Step 4: Setup

void setup() {

  for (int i = 0; i < 12; i++) {
    pinMode(leds[i], OUTPUT);
  }

  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(buzzerPin, HIGH); // buzzer OFF
}

// Step 5: Main loop

void loop() {

  // If game is won, wait for button press to restart
  if (gameWon) {
    waitForRestart();
    return;
  }

  animateStage();
  checkButton();
}

// Step 6: LED animations

void animateStage() {

  if (millis() - lastMove < speedMs) return;
  lastMove = millis();

  for (int i = 0; i < 12; i++) {
    if (!locked[i]) {
      digitalWrite(leds[i], LOW);
    }
  }

  int base = stage * 3;
  digitalWrite(leds[base + indexPos], HIGH);

  indexPos++;
  if (indexPos > 2) indexPos = 0;
}

// Step 7: Button check

void checkButton() {

  bool current = digitalRead(buttonPin);

  if (current && !lastButton) {

    int base = stage * 3;
    int lit = base + indexPos - 1;

    if (lit < base) {
      lit = base + 2;
    }

    if (lit == base + 1) {
      lockLED(lit);
    }
  }

  lastButton = current;
}

// Step 8: Lock LED and advance

void lockLED(int ledIndex) {

  locked[ledIndex] = true;
  digitalWrite(leds[ledIndex], HIGH);

  beep();

  stage++;
  indexPos = 0;

  if (stage == 4) {
    winMelody();
    gameWon = true;
  }
}

// Step 9: Restart

void waitForRestart() {

  bool current = digitalRead(buttonPin);

  // Detect NEW button press
  if (current && !lastButton) {
    resetGame();
  }

  lastButton = current;
}

// Step 10: Reset game

void resetGame() {

  // Turn off all LEDs
  for (int i = 0; i < 12; i++) {
    locked[i] = false;
    digitalWrite(leds[i], LOW);
  }

  stage = 0;
  indexPos = 0;
  lastMove = 0;
  gameWon = false;

  digitalWrite(buzzerPin, HIGH); // buzzer OFF
}

// Step 11: sound fonctions

void beep() {
  digitalWrite(buzzerPin, LOW);
  delay(80);
  digitalWrite(buzzerPin, HIGH);
}

void winMelody() {

  int duration;

  for (int i = 0; i < songLength; i++) {
    duration = beats[i] * tempo;

    if (notes[i] == ' ') {
      delay(duration);
    } else {
      tone(buzzerPin, frequency(notes[i]), duration);
      delay(duration);
    }

    delay(tempo / 10);
  }

  noTone(buzzerPin);
  digitalWrite(buzzerPin, HIGH);
}

// Step 12: note and frequency

int frequency(char note) {

  const int numNotes = 8;
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};

  for (int i = 0; i < numNotes; i++) {
    if (names[i] == note) {
      return frequencies[i];
    }
  }
  return 0;
}
