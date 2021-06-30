#include <SPI.h>
#include "mcp_can.h"

const int spiCSPin = 10;

const int redPin = 6;
const int greenPin = 5;
const int bluePin = 3;

int blinkTime = 0;
boolean blink = false;
long blinkMillis = 0;


int onTime = 0;
boolean on = false;
boolean constantOn = false;
long onMillis = 0;

int redIntesitivity = 0;
int greenIntesitivity = 0;
int blueIntesitivity = 0;

boolean ledState = false;

boolean changed = false;

MCP_CAN CAN(spiCSPin);

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  analogWrite(redPin, redIntesitivity);
  analogWrite(greenPin, greenIntesitivity);
  analogWrite(bluePin, blueIntesitivity);
  while (CAN_OK != CAN.begin(CAN_500KBPS)) {
    Serial.println("CAN BUS Init Failed");
    delay(100);
  }
  Serial.println("CAN BUS  Init OK!");
}


void loop() {
  unsigned char len = 0;
  unsigned char buf[8];

  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    CAN.readMsgBuf(&len, buf);

    unsigned long canId = CAN.getCanId();

    Serial.println("-----------------------------");
    Serial.print("Data from ID: 0x");
    Serial.println(canId, HEX);

    for (int i = 0; i < len; i++) {
      Serial.print(buf[i]);
      Serial.print("\t");
    }
    Serial.println();
    if (len > 4) {
      redIntesitivity = buf[0];
      greenIntesitivity = buf[1];
      blueIntesitivity = buf[2];

      onTime = buf[3];
      constantOn = onTime == 0 ? true : false;

      blinkTime = buf[4];
      blink = blinkTime == 0 ? false : true;
      on = true;
      analogWrite(redPin, redIntesitivity);
      analogWrite(greenPin, greenIntesitivity);
      analogWrite(bluePin, blueIntesitivity);
      onMillis = millis();
      ledState = false;
    }
  }

  unsigned long currentMillis = millis();

  if (on && !constantOn) {
    if (currentMillis - onMillis > (onTime * 1000)) {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
      ledState = false;
      on = false;
    }
  }

  if (blink && (on || constantOn))
  {
    if (currentMillis - blinkMillis > (blinkTime * 100)) {
      blinkMillis = currentMillis;
      if (ledState) {
        analogWrite(redPin, 0);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0);
        ledState = false;
      }
      else {
        analogWrite(redPin, redIntesitivity);
        analogWrite(greenPin, greenIntesitivity);
        analogWrite(bluePin, blueIntesitivity);
        ledState = true;
      }

    }
  }



}
