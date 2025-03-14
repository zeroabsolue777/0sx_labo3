#include <Wire.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2); // Adresse I2C 0x27, écran 16x2

const int LED_PIN = 8;
const int butt = 2;
const int X=A1;
const int Y=A2;

float R1 = 10000; // Résistance
float logR2, R2, T, Tc;
float c1 = 1.129148e-03, c2 = 2.34125e-04, c3 = 8.76741e-08; // Coefficients Steinhart-Hart

int systemState = 0; // 0 = OFF, 1 = ON

unsigned long lastTempUpdate = 0;
unsigned long lastSerialUpdate = 0;
unsigned long lastButtonPress = 0;
unsigned long lastLcdUpdate = 0;

int currentPage = 0;
bool buttonState = HIGH, lastButtonState = HIGH;

byte numero[8] = {
  0B11100,
  0B10000,
  0B11100,
  0B10100,
  0B11111,
  0B00101,
  0B00101,
  0B00111
};

byte degre[8] = {
  0B01000,
  0B10100,
  0B01000,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

void affichage(){
  static unsigned long wait =3000;

   lcd.createChar(0, numero);

  while(millis() < wait){

    lcd.print("Leon perez");

    
    lcd.setCursor(0,1);
    lcd.print(char(0));

   
    lcd.setCursor(14,1);
    lcd.print("60");
  }
  
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(butt, INPUT_PULLUP); // Activer la résistance de pull-up interne

    lcd.begin(16, 2);
  
    lcd.backlight();
    // Affichage du démarrage
      affichage();
     lcd.clear();
}

void Temperature() {

    static unsigned long wait = 500;

    if (millis() - lastTempUpdate >= wait) { // Mise à jour toutes les 500ms
        lastTempUpdate = millis();

        int thermistorValue = analogRead(A0);
        R2 = R1 * (1023.0 / thermistorValue - 1.0);
        logR2 = log(R2);
        T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
        Tc = T - 273.15;

        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(Tc);
        lcd.print(" C  "); // Espaces pour éviter les restes d'affichage

        lcd.setCursor(0, 1);
        if (Tc > 25) {
            digitalWrite(LED_PIN, HIGH); 
            lcd.print("AC: ON  ");
            systemState = 1;
        } else if (Tc < 24) {
            digitalWrite(LED_PIN, LOW);
            lcd.print("AC: OFF ");
            systemState = 0;
        }
    }
}

void Joystick() {

     static unsigned long wait = 200;

    if (millis() - lastLcdUpdate >= 200) { // Mise à jour toutes les 200ms
        lastLcdUpdate = millis();

        int xValue = analogRead(X);
        int yValue = analogRead(Y);

        int speed = map(yValue, 0, 1023, -120, 120);
        int direction = map(xValue, 0, 1023, -90, 90);

        lcd.setCursor(0, 0);

       
        lcd.print(speed < 0 ? "Recule: " : "Avance: "); 
        lcd.print(speed);
        
        lcd.print("km/h  "); 

        lcd.setCursor(0, 1);
        lcd.print(direction < 0 ? "Gauche:" : "Droite:  ");
        lcd.print(direction);
        lcd.print(" ");
        lcd.createChar(1, degre);
        lcd.setCursor(11,1);
        lcd.print(char(1));
        
    }
  
}

void Button() {

  buttonState = digitalRead(butt);

    if (buttonState == LOW && lastButtonState == HIGH) {
        if (millis() - lastButtonPress >= 200) { // Anti-rebond
            lastButtonPress = millis();
            currentPage = (currentPage + 1) % 2;
            lcd.clear();
        }
    }
    lastButtonState = buttonState;

    if (currentPage == 0) {
        Temperature();
    } else {
        Joystick();
    }
}


void loop() {
  
    Button();
    static unsigned long wait = 100;
    if (millis() - lastSerialUpdate >= wait) { // Envoi toutes les 100ms
        lastSerialUpdate = millis();

        int xValue = analogRead(X);
        int yValue = analogRead(Y);

        Serial.print("etd:2411160,x:");
        Serial.print(xValue);
        Serial.print(",y:"); 
        Serial.print(yValue);
        Serial.print(",sys:"); 
        Serial.println(systemState);
    }
}
