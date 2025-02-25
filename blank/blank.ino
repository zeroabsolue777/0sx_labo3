#define BTN_PIN 2

unsigned long currentTime = 0;

void setup() {
  Serial.begin(115200);

}

void loop() {
  currentTime = millis();

}
