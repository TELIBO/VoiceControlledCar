#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

VR myVR(2, 3);  // Voice Recognition Module connected to pins 2 (RX) and 3 (TX)

int RightMotorForward = 4;
int RightMotorBackward = 5;
int LeftMotorForward = 6;
int LeftMotorBackward = 7;

#define left    (0)
#define right   (1)
#define forward (2)
#define backward (3)
#define stopp   (4)

uint8_t buf[64];

void setup() {
  myVR.begin(9600);
  Serial.begin(115200);
  Serial.println("Voice Control Car");

  pinMode(LeftMotorForward, OUTPUT);
  pinMode(RightMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);

  if (myVR.clear() == 0) {
    Serial.println("Recognizer cleared.");
  } else {
    Serial.println("Voice Recognition Module not found.");
    Serial.println("Please check the connection and restart Arduino.");
    while (1)
      ;
  }

  loadVoiceCommand(left, "left");
  loadVoiceCommand(right, "right");
  loadVoiceCommand(forward, "forward");
  loadVoiceCommand(backward, "backward");
  loadVoiceCommand(stopp, "stop");
}

void loop() {
  int ret = myVR.recognize(buf, 50);
  if (ret > 0) {
    handleVoiceCommand(buf[1]);
    delay(500);  // Adjust this delay if needed
    MotorStop(); // Stop the motors after a short delay
    printVR(buf);
  }
}

void loadVoiceCommand(uint8_t command, const char* commandName) {
  if (myVR.load(command) >= 0) {
    Serial.print(commandName);
    Serial.println(" loaded");
  } else {
    Serial.println("Failed to load voice command");
  }
}

void handleVoiceCommand(uint8_t command) {
  switch (command) {
    case left:
      TurnLeft();
      break;
    case right:
      TurnRight();
      break;
    case forward:
      MotorForward();
      break;
    case backward:
      MotorBackward();
      break;
    case stopp:
      MotorStop();
      break;
    default:
      Serial.println("Unknown command");
      break;
  }
}

void MotorForward() {
  digitalWrite(RightMotorForward, HIGH);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);
}

void MotorBackward() {
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, HIGH);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, HIGH);
}

void TurnRight() {
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, HIGH);
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);
}

void TurnLeft() {
  digitalWrite(RightMotorForward, HIGH);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, HIGH);
}

void MotorStop() {
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);
}

void printVR(uint8_t *buf) {
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if (buf[0] == 0xFF) {
    Serial.print("NONE");
  } else if (buf[0] & 0x80) {
    Serial.print("UG ");
    Serial.print(buf[0] & (~0x80), DEC);
  } else {
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");

  if (buf[3] > 0) {
    printSignature(buf + 4, buf[3]);
  } else {
    Serial.print("NONE");
  }

  Serial.println("\r\n");
}

void printSignature(uint8_t *buf, int len) {
  for (int i = 0; i < len; i++) {
    if (buf[i] > 0x19 && buf[i] < 0x7F) {
      Serial.write(buf[i]);
    } else {
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}
