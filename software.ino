software.ino#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define POT A0         // Potentiometer simulating current sensor
#define RELAY 8        // Simulated Relay output (LED)
#define BUZZER 9       // Buzzer pin
#define BUTTON 7       // Reset button

Adafruit_SSD1306 display(128, 64, &Wire, -1);
bool overload = false;

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(RELAY, HIGH); // Relay ON by default

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  int sensorValue = analogRead(POT); // Simulated current

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Current (sim): ");
  display.println(sensorValue);
  display.setCursor(0, 20);

  if (!overload && sensorValue > 600) {
    digitalWrite(RELAY, LOW);  // Cut relay (LED OFF)
    tone(BUZZER, 1000);         // Start buzzer
    display.print("Status: OVERLOAD!");
    overload = true;
  }
  else if (!overload) {
    display.print("Status: Normal");
  }

  if (overload && digitalRead(BUTTON) == LOW) {
    overload = false;
    digitalWrite(RELAY, HIGH); // Relay ON
    noTone(BUZZER);            // Stop buzzer
    delay(300);                // Debounce delay
  }

  display.display();
  delay(200); // Small delay for readability
}