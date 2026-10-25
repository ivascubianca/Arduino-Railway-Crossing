#include <Servo.h>
#include <TM1637Display.h>

// ===== PINI =====
#define SERVO_PIN 9
#define BUZZER 4

#define LED_ROSU 5
#define LED_GALBEN 6
#define LED_VERDE 7

#define BTN_UP 11
#define BTN_DOWN 12

#define CLK 8
#define DIO 10

// ===== CONSTANTE =====
#define SERVO_JOS 33
#define SERVO_SUS 140
#define GALBEN_TIME 5000UL  

Servo servoMotor;
TM1637Display display(CLK, DIO);

enum State { ROSU, GALBEN, VERDE };
State state = VERDE;

unsigned long stateStartTime;
unsigned long lastServoMove;

int servoPos = SERVO_SUS;
int servoTarget = SERVO_SUS;
int servoStep = 0;

// ================= SETUP =================
void setup() {
  pinMode(LED_ROSU, OUTPUT);
  pinMode(LED_GALBEN, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(SERVO_SUS);   

  display.setBrightness(0x0f);

  setState(VERDE);
}

// ================= LOOP =================
void loop() {
  updateDisplay();
  handleButtons();
  handleServo();
  handleStateTimeout();
}

// ================= FUNCȚII =================

void handleButtons() {
  if (digitalRead(BTN_DOWN) == LOW && state == VERDE) {
    startMove(SERVO_JOS);
    delay(200);
  }

  if (digitalRead(BTN_UP) == LOW && state == ROSU) {
    startMove(SERVO_SUS);
    delay(200);
  }
}

void startMove(int target) {
  servoTarget = target;
  servoStep = (servoTarget > servoPos) ? 1 : -1;
  lastServoMove = millis();
  setState(GALBEN);
}

void handleServo() {
  if (state != GALBEN) return;

  unsigned long interval = GALBEN_TIME / abs(SERVO_SUS - SERVO_JOS);

  if (millis() - lastServoMove >= interval) {
    servoPos += servoStep;
    servoMotor.write(servoPos);
    lastServoMove = millis();
  }
}

void handleStateTimeout() {
  if (state == GALBEN && millis() - stateStartTime >= GALBEN_TIME) {
    if (servoTarget == SERVO_JOS) setState(ROSU);
    if (servoTarget == SERVO_SUS) setState(VERDE);
  }
}

void updateDisplay() {
  display.showNumberDec((millis() - stateStartTime) / 1000, true);
}

void setState(State newState) {
  digitalWrite(LED_ROSU, LOW);
  digitalWrite(LED_GALBEN, LOW);
  digitalWrite(LED_VERDE, LOW);
  noTone(BUZZER);

  state = newState;
  stateStartTime = millis();

  if (state == ROSU) digitalWrite(LED_ROSU, HIGH);
  if (state == VERDE) digitalWrite(LED_VERDE, HIGH);

  if (state == GALBEN) {
    digitalWrite(LED_GALBEN, HIGH);
    tone(BUZZER, 2000);   
  }
}