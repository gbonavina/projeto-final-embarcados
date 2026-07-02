#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// MFRC522 pins — Arduino Mega (hardware SPI: 50 MISO, 51 MOSI, 52 SCK)
#define RST_PIN 5
#define SS_PIN  53

// Servo signal pin
#define SERVO_PIN 6

// Passive buzzer pin (PWM)
#define BUZZER_PIN 7

// HC-SR04 ultrasonic sensor pins
#define TRIG_PIN 22
#define ECHO_PIN 23
#define MIN_OPEN_DISTANCE_CM 5

#define ANGLE_CLOSED 180
#define ANGLE_OPEN   90

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo lidServo;

#define BUZZER_ON  128
#define BUZZER_OFF 0

bool isOpen = false;

void beep(int durationMs) {
  analogWrite(BUZZER_PIN, BUZZER_ON);
  delay(durationMs);
  analogWrite(BUZZER_PIN, BUZZER_OFF);
}

void playOpenSong() {
  beep(120);
  delay(60);
  beep(120);
  delay(60);
  beep(250);
}

void printTagUID() {
  Serial.print(F("UID do Usuario: "));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      Serial.print('0');
    }
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
}

long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    return -1;
  }

  return duration * 0.034 / 2;
}

bool isObstacleTooClose() {
  long distance = readDistanceCm();

  Serial.print(F("Distancia: "));
  Serial.print(distance);
  Serial.println(F(" cm"));

  return distance <= MIN_OPEN_DISTANCE_CM;
}

void openCan() {
  lidServo.write(ANGLE_OPEN);
  isOpen = true;
  Serial1.write(1);
  playOpenSong();
}

void closeCan() {
  lidServo.write(ANGLE_CLOSED);
  isOpen = false;
  Serial1.write(0);
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  lidServo.attach(SERVO_PIN);
  closeCan();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  printTagUID();

  if (isOpen) {
    closeCan();
  } else if (isObstacleTooClose()) {
    Serial.println(F("Abertura bloqueada: objeto a 5 cm ou menos"));
  } else {
    openCan();
  }

  delay(300);
  while (mfrc522.PICC_IsNewCardPresent()) {
    delay(50);
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(200);
}
