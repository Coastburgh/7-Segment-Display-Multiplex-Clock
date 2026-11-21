#include <EEPROM.h>

int a = 2;
int b = 3;
int c = 4;
int d = 5;
int digit1 = 6;
int digit2 = 7;
int digit3 = 8;
int digit4 = 9;

int hours = 0;
int minutes = 0;

bool adjustmentMode = false;
int adjustmentSection = 0; // 0 - hours, 1 - minutes, 2 - exit adjustment mode

int adjustButton = 10;   // Adjustment button
int decreaseButton = 11; // Button to decrease the value of the current section
int increaseButton = 12; // Button to increase the value of the current section

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(a + i, OUTPUT);
  }
  Serial.begin(19200);

  pinMode(adjustButton, INPUT_PULLUP);
  pinMode(increaseButton, INPUT_PULLUP);
  pinMode(decreaseButton, INPUT_PULLUP);

  // Read saved values from EEPROM memory
  hours = EEPROM.read(0);
  minutes = EEPROM.read(1);
}

void display(int position, int digit) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(digit1 + i, HIGH);
  }

  int mask = 1;
  for (int i = 0; i < 4; i++) {
    if (digit & mask)
      digitalWrite(a + i, 1);
    else
      digitalWrite(a + i, 0);
    mask = mask << 1;
  }

  digitalWrite(position, LOW);
  delay(5);
}

void displayClock() {
  int dig1 = minutes % 10;
  display(digit1, dig1);
  int dig2 = minutes / 10;
  display(digit2, dig2);
  dig1 = hours % 10;
  display(digit3, dig1);
  dig2 = hours / 10;
  display(digit4, dig2);
}

long currentTime = 0;
long nextTime = 0;

void loop() {
  if (!adjustmentMode) {
    displayClock();
    currentTime = millis();
    if (currentTime - nextTime > 60000) {
      minutes += 1;
      nextTime = currentTime;
    }
    if (minutes == 60) {
      hours += 1;
      minutes = 0;
    }
    if (hours == 24) {
      hours = 0;
    }

    // Check if the adjustment button was pressed
    if (digitalRead(adjustButton) == LOW) {
      adjustmentMode = true;
      adjustmentSection = 0;
      delay(200); // Small delay to avoid false button readings
    }
  } else {
    // Adjustment mode activated
    // Freeze the time display
    displayClock();

    // Check if the increase button was pressed
    if (digitalRead(increaseButton) == LOW) {
      delay(200); // Small delay to avoid false button readings

      // Increase the current section of the clock
      switch (adjustmentSection) {
        case 0: // Hour adjustment
          hours += 1;
          if (hours == 24) {
            hours = 0;
          }
          break;
        case 1: // Minute adjustment
          minutes += 1;
          if (minutes == 60) {
            minutes = 0;
          }
          break;
        case 2: // Exit adjustment mode
          adjustmentMode = false;
          EEPROM.write(0, hours);
          EEPROM.write(1, minutes);
          break;
      }
    }

    // Check if the decrease button was pressed
    if (digitalRead(decreaseButton) == LOW) {
      delay(200); // Small delay to avoid false button readings

      // Decrease the current section of the clock
      switch (adjustmentSection) {
        case 0: // Hour adjustment
          hours -= 1;
          if (hours < 0) {
            hours = 23;
          }
          break;
        case 1: // Minute adjustment
          minutes -= 1;
          if (minutes < 0) {
            minutes = 59;
          }
          break;
      }
    }

    // Check if the adjustment button was pressed again to switch to the next section or exit adjustment mode
    if (digitalRead(adjustButton) == LOW) {
      delay(200); // Small delay to avoid false button readings
      if (adjustmentSection == 2) {
        adjustmentMode = false; // Exit adjustment mode
      } else {
        adjustmentSection++; // Switch to the next section (hours, minutes) or exit adjustment mode
      }
    }
  }
}