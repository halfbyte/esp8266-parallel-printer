#include <Arduino.h>

#define PIN_SHIFT_OUT D1
#define PIN_SHIFT_CLK D2
#define PIN_OUTPUT_CLK D3

#define PIN_STROBE D7
#define PIN_ACK D5
#define PIN_BUSY D6
#define STROBEWAIT 10

int ackIn = 0;
int busyCount = 0;

void printByte(byte inByte) {
  while(digitalRead(PIN_BUSY) == HIGH) {
    // wait for busy to go low
  }
  Serial.printf("writing byte %d\n", inByte);
  digitalWrite(PIN_OUTPUT_CLK, LOW);
  digitalWrite(PIN_SHIFT_CLK, LOW);
  shiftOut(PIN_SHIFT_OUT, PIN_SHIFT_CLK, MSBFIRST, inByte);
  delayMicroseconds(10);
  digitalWrite(PIN_OUTPUT_CLK, HIGH);
  delayMicroseconds(10);
  ackIn = 0;
  busyCount = 0;
  digitalWrite(D4, HIGH);
  digitalWrite(PIN_STROBE, LOW);       // strobe nStrobe to input data bits
  delayMicroseconds(STROBEWAIT);
  digitalWrite(PIN_STROBE, HIGH);
  delayMicroseconds(STROBEWAIT);
  while(digitalRead(PIN_BUSY) == HIGH) {
    // wait for busy line to go low
  }
  if (ackIn) {
    Serial.printf("ACK %dxbusy\n", busyCount);
  }
  Serial.printf("BusyCount %d\n", busyCount);
  delay(1);
}

void printLine(byte *line) {
  int len = strlen((char *)line);
  int i;
  Serial.printf("%d bytes to print\n", len);
  for(i=0;i<len;i++) {
    printByte(line[i]);
  }
  printByte(10);
  printByte(13);
}

void resetPrinter() {
  Serial.println("Reseting printer...");
  printByte(27); // reset printer
  printByte('@');
  Serial.println("Printer Reset");
}

void ICACHE_RAM_ATTR handleAck() {
  ackIn = 1;
}

void ICACHE_RAM_ATTR handleBusy() {
  digitalWrite(D4, LOW);
  busyCount++;
}

void setup() {
  byte message[] = "Hello Message";
  Serial.begin(115200);
  pinMode(D0, INPUT_PULLDOWN_16);
  pinMode(D4, OUTPUT);
  pinMode(PIN_SHIFT_OUT, OUTPUT);
  pinMode(PIN_SHIFT_CLK, OUTPUT);
  pinMode(PIN_OUTPUT_CLK, OUTPUT);
  pinMode(PIN_STROBE, OUTPUT);
  digitalWrite(PIN_STROBE, HIGH);
  digitalWrite(PIN_SHIFT_CLK, LOW);
  digitalWrite(PIN_OUTPUT_CLK, LOW);

  pinMode(PIN_ACK, INPUT);
  pinMode(PIN_BUSY, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_ACK), handleAck, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BUSY), handleBusy, RISING);

  Serial.println("Setup Done");
  resetPrinter();
  printByte(7); // Bell
  delay(5000);

  //printByte('a'); // An a
  printLine(message);
}



void loop() {
  // printByte(7);
  delay(1000);
}