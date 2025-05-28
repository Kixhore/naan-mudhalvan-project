#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
const int currentSensorPin = A0;
const int relayPin = 6;
const int buzzerPin = 8;
const int ledPin = 7;
const int buttonPin = 9;

// Constants
const float voltage = 220.0;         // Fixed voltage for power calculation
const float thresholdCurrent = 10.0; // Threshold in Amps

// Globals
bool alertState = false;

void setup() {
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Using internal pull-up

  digitalWrite(relayPin, HIGH); // Relay ON initially
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);

  // OLED initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Smart Energy Monitor");
  display.display();
  delay(2000);
}

void loop() {
  float current = readCurrent();  // Read current from ACS712
  float power = voltage * current;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Current: ");
  display.print(current, 2);
  display.println(" A");

  display.print("Power: ");
  display.print(power, 2);
  display.println(" W");

  if (current > thresholdCurrent) {
    triggerAlert(current);
  } else if (!alertState) {
    digitalWrite(relayPin, HIGH);  // Keep machine ON
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    display.println("Status: Normal");
  }

  display.display();

  // Check for manual reset
  if (alertState && digitalRead(buttonPin) == LOW) {
    delay(200); // Debounce
    alertState = false;
    digitalWrite(relayPin, HIGH);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
  }

  delay(500);
}

float readCurrent() {
  int sensorValue = analogRead(currentSensorPin);
  float voltageOut = (sensorValue * 5.0) / 1023.0;
  float current = (voltageOut - 2.5) / 0.185; // For ACS712-5A use 0.185, for 20A use 0.100
  if (current < 0) current = -current; // remove noise below 0
  return current;
}

void triggerAlert(float current) {
  alertState = true;
  digitalWrite(relayPin, LOW);   // Turn off machine
  digitalWrite(buzzerPin, HIGH); // Sound buzzer
  digitalWrite(ledPin, HIGH);    // Turn on alert LED
  display.println("ALERT! Overcurrent");
}
