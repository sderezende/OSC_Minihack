
#include <LiquidCrystal.h>

//Note Definitions
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_E4  330
#define NOTE_F4  349

//Pins
int red_pin = 6;
int green_pin = 5;
int blue_pin = 3;
int buzzer_pin = 4;
int button_pin = 2;

//LCD Setup 
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

//Timer
unsigned long last_trigger = 0;
const unsigned long interval = 15000; 
bool alarm_on = false;

//Melody
int jaws_melody[] = {
  NOTE_E2, NOTE_F2, NOTE_E3, NOTE_F3, 
  NOTE_E2, NOTE_F2, NOTE_E3, NOTE_F3,
  NOTE_E4, NOTE_F4, NOTE_E3, NOTE_F3,
  NOTE_E2, NOTE_F2, NOTE_E2  
};

int jaws_durations[] = {
  500, 500, 400, 400,
  350, 350, 300, 300,
  250, 250, 300, 300,
  600, 600, 1200  
};


void setup() {
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Next pills in:");

  Serial.begin(9600);
  last_trigger = millis();
}

//Set LED color
void setColor(int r, int g, int b) {
  analogWrite(red_pin, r);
  analogWrite(green_pin, g);
  analogWrite(blue_pin, b);
}

//Play Jaws Theme once with flashing colors
bool playJawsTheme() {
  int nNotes = sizeof(jaws_melody) / sizeof(jaws_melody[0]);
  for (int i = 0; i < nNotes; i++) {
    int noteDuration = jaws_durations[i];
    int note = jaws_melody[i];

    if (note > 0) {
      tone(buzzer_pin, note, noteDuration);
    } else {
      noTone(buzzer_pin);
    }

    // Flash rainbow while note plays
    for (int t = 0; t < noteDuration; t += 100) {
      setColor(random(0, 256), random(0, 256), random(0, 256));
      delay(100);

      // Button check
      if (digitalRead(button_pin) == LOW) {
        noTone(buzzer_pin);
        return false; 
      }
    }
    noTone(buzzer_pin);
    delay(noteDuration * 0.2);
  }
  return true; 
}

//Main Loop
void loop() {
  unsigned long now = millis();
  unsigned long elapsed = now - last_trigger;
  int secondsLeft = max(0, (int)((interval - elapsed) / 1000));

  if (!alarm_on) {
    // Update countdown on LCD
    lcd.setCursor(0, 1);
    lcd.print("       ");
    lcd.setCursor(0, 1);
    lcd.print(secondsLeft);
    lcd.print("s   ");

    setColor(255, 30, 0); 
    noTone(buzzer_pin);

    // Trigger alarm when time runs out
    if (elapsed >= interval) {
      alarm_on = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Take pills!");
      Serial.println("Alarm triggered!");
    }
    return;
  }

  //Alarm mode
  while (alarm_on) {
    bool keepGoing = playJawsTheme();
    if (!keepGoing) {
      alarm_on = false;
      setColor(255, 30, 0); 
      last_trigger = millis(); 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Next pills in:");
      Serial.println("Button pressed, alarm off, timer reset!");
    }
  }
}



