/*
BOARD: ARDUINO UNO
VERSION: 1
PINS:
  Taster: D10 - Blau, D11 - Gruen, D12 - Gelb, D13 - Rot
  NeoPixel: D6
  Servo: D7

  SCL: A0
  SDA: A1
*/

// LIBARIES
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

// DEFINE PIXELRING
#define PIN_RING 6
#define NUMPIXELS 12
Adafruit_NeoPixel ring(NUMPIXELS, PIN_RING, NEO_GRB + NEO_KHZ800);

// DEFINE colors for Pixelring
uint32_t farbeRot = ring.Color(255, 0, 0);
uint32_t farbeGelb = ring.Color(255, 255, 0);
uint32_t farbeGruen = ring.Color(0, 255, 0);
uint32_t farbeBlau = ring.Color(0, 0, 255);
uint32_t farbeWeiss = ring.Color(255, 255, 255);

// DEFINE SERVO
#define PIN_SERVO 7
Servo servo_goal;

// DEFINE LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DEFINE BUTTONS
#define PIN_BLAU 10
#define PIN_GRUEN 11
#define PIN_GELB 12
#define PIN_ROT 13

// GLOBALE VARIABLEN
int loesung[7];
int test[6];
int eingabe[6];
int counterEingabe = 0;
int counterRing = 0;
unsigned long millisRing = 0;
unsigned long millisTaster = 0;


void setup() {
  // put your setup code here, to run once:

  // Initialize LED, SERVO, DISPLAY
  ring.begin();
  ring.setBrightness(15);

  servo_goal.attach(PIN_SERVO);
  servo_goal.write(30);

  lcd.init();
  lcd.backlight();

  // TASTER
  pinMode(PIN_BLAU, INPUT);
  pinMode(PIN_GRUEN, INPUT);
  pinMode(PIN_GELB, INPUT);
  pinMode(PIN_ROT, INPUT);

  // TESTS
  ring_test();
  // servo_test();
  // lcd_test();
  // Serial.begin(9600);

  // LOSUNG FESTLEGEN UND test BEREIT STELLEN
  randomSeed(analogRead(0));
  for (int i = 0; i < 6; i++) {
    loesung[i] = random(1, 13);
    test[i] = loesung[i];
    eingabe[i] = 0;
    //Ursprungslösung 5-8:
    if (test[i] > 4) {
      test[i] = test[i] - 4;
    }
    //Ursprungslösung 9-12:
    if (test[i] > 4) {
      test[i] = test[i] - 2;
    }
    if (test[i] > 4) {
      test[i] = test[i] - 4;
    }
  }
  // Loesung[6] generiert eine Pause in der Lichtsequenz
  loesung[6] = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  // TEST
  // button_test();
  if (millis() - millisRing > 2000) {
    ringNext();
  }

  if (millis() - millisTaster > 1000) {
    if (digitalRead(PIN_BLAU)) {
      taster(0);
    }

    if (digitalRead(PIN_GRUEN)) {
      taster(1);
    }

    if (digitalRead(PIN_GELB)) {
      taster(2);
    }

    if (digitalRead(PIN_ROT)) {
      taster(3);
    }
  }
  if (counterRing == 7) {
    counterRing = 0;
  }

  if (counterEingabe == 6) {
    pruefeLoesung();
  }
}

// EIGENE TEILPROGRAMME

void ringNext() {
  ring.clear();
  ring.show();
  delay(100);

  switch (loesung[counterRing]) {
    case 1:
      ring.fill(farbeBlau, 0, 3);
      break;

    case 2:
      ring.fill(farbeGruen, 3, 3);
      break;

    case 3:
      ring.fill(farbeGelb, 6, 3);
      break;

    case 4:
      ring.fill(farbeRot, 9, 3);
      break;

    case 5:
      ring.fill(farbeBlau, 0);
      break;

    case 6:
      ring.fill(farbeGruen, 0);
      break;

    case 7:
      ring.fill(farbeGelb, 0);
      break;

    case 8:
      ring.fill(farbeRot, 0);
      break;

    case 9:
      ring.fill(farbeWeiss, 0, 3);
      break;

    case 10:
      ring.fill(farbeWeiss, 3, 3);
      break;

    case 11:
      ring.fill(farbeWeiss, 6, 3);
      break;

    case 12:
      ring.fill(farbeWeiss, 9, 3);
      break;

    default:
      break;
  }
  ring.show();
  counterRing++;
  millisRing = millis();
}

void taster(int value) {
  int row = 0;
  int pos = 0 + counterEingabe * 5;
  if (counterEingabe > 2) {
    row = 1;
    pos = pos - 15;
  }
  /* Debugging
  Serial.print("Row: ");
  Serial.print(row);
  Serial.print(" Position: ");
  Serial.println(pos);
  */

  eingabe[counterEingabe] = value + 1;
  lcd.setCursor(pos, row);

  switch (value) {
    case 0:
      lcd.print("BLAU");
      // Debug
      // Serial.println("Blau");
      break;

    case 1:
      lcd.print("GR");
      lcd.write(245);
      lcd.print("N");
      // Debug
      // Serial.println("Grün");
      break;

    case 2:
      lcd.print("GELB");
      // Debug
      // Serial.println("Gelb");
      break;

    case 3:
      lcd.print("ROT");
      // Debug
      // Serial.println("Rot");
      break;

    default:
      lcd.print("ERR");
      break;
  }

  counterEingabe++;
  millisTaster = millis();
}

void pruefeLoesung() {
  bool richtig = true;
  for (int i = 0; i < 6; i++) {
    if (loesung[i] > 4 && loesung[i] < 9 && test[i] != eingabe[i]) {
      continue;
    } else if ((loesung[i] <= 4 || loesung[i] >= 9) && test[i] == eingabe[i]) {
      continue;
    } else {
      richtig = false;
      break;
    }
  }

  // wenn richtig -> Ring Rainbow, Öffne Servo, kill programm
  if (richtig) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RICHTIG");
    servo_goal.write(30);
    ring.clear();
    ring.fill(farbeWeiss, 0);
    ring.show();
    for (int i = 0; i < 6; i++) {
      delay(250);
      ring.clear();
      ring.show();
      delay(250);
      ring.fill(farbeWeiss, 0);
      ring.show();
    }
    delay(250);
    ring.clear();
    ring.show();
    for(;;){

    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FALSCH");
    ring.clear();
    ring.show();
    counterEingabe = 0;
    counterRing = 0;
    delay(3000);
    lcd.clear();
  }
  // wenn falsch -> Lösche Display, reset counter

  //!!! DUMMY !!!
  /*
    lcd.clear();
  counterEingabe = 0;
  Serial.println("Lösung:");
  for (int i = 0; i < 6; i++) {
    Serial.print(loesung[i]);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.println("Test:");
  for (int i = 0; i < 6; i++) {
    Serial.print(test[i]);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.println("Eingabe:");
  for (int i = 0; i < 6; i++) {
    Serial.print(eingabe[i]);
    Serial.print(" ");
  }
  */
}



// UNIT TESTS

void ring_test() {
  ring.clear();
  ring.show();
  delay(500);
  ring.fill(farbeBlau, 0, 3);
  ring.fill(farbeGruen, 3, 3);
  ring.fill(farbeGelb, 6, 3);
  ring.fill(farbeRot, 9, 3);
  ring.show();
  delay(2500);
  ring.clear();
  ring.show();
}

void servo_test() {
  servo_goal.write(0);
  delay(500);
  servo_goal.write(90);
  delay(500);
  servo_goal.write(180);
  delay(500);
  servo_goal.write(90);
}

void lcd_test() {
  lcd.setCursor(0, 0);  //Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile.
  lcd.print("TESTTESTTEST1234");
  lcd.setCursor(0, 1);  // In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile.
  lcd.print("TESTTESTTESTTEST");
  delay(2000);
  lcd.noBacklight();
  delay(2000);
  lcd.backlight();
  delay(2000);
  lcd.clear();
}

void button_test() {
  if (digitalRead(PIN_BLAU)) {
    lcd.setCursor(0, 0);
    lcd.print("TASTER BLAU");
    lcd.setCursor(0, 1);
    lcd.print("GEDR");
    lcd.write(245);
    lcd.print("ECKT");
    delay(1000);
    lcd.clear();
  }

  if (digitalRead(PIN_GRUEN)) {
    lcd.setCursor(0, 0);
    lcd.print("TASTER GRN");
    lcd.write(245);
    lcd.print("N");
    lcd.setCursor(0, 1);
    lcd.print("GEDR");
    lcd.write(245);
    lcd.print("ECKT");
    delay(1000);
    lcd.clear();
  }

  if (digitalRead(PIN_GELB)) {
    lcd.setCursor(0, 0);
    lcd.print("TASTER GELB");
    lcd.setCursor(0, 1);
    lcd.print("GEDR");
    lcd.write(245);
    lcd.print("ECKT");
    delay(1000);
    lcd.clear();
  }

  if (digitalRead(PIN_ROT)) {
    lcd.setCursor(0, 0);
    lcd.print("TASTER ROT");
    lcd.setCursor(0, 1);
    lcd.print("GEDR");
    lcd.write(245);
    lcd.print("ECKT");
    delay(1000);
    lcd.clear();
  }
}
