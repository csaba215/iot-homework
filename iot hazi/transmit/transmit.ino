#include <SPI.h>
#include <mcp_can.h>

const int spiCSPin = 10;
int ledHIGH    = 1;
int ledLOW     = 0;

boolean redblue = false;

boolean usenino = false;
long blinkMillis = 0;

char buf[80];
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

MCP_CAN CAN(spiCSPin);

void setup()
{
  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_500KBPS))
  {
    Serial.println("CAN BUS init Failed");
    delay(100);
  }
  Serial.println("CAN BUS Shield Init OK!");
}

int readline(int readch, char *buffer, int len) {
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\r': // Ignore CR
        break;
      case '\n': // Return on new-line
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len - 1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  return 0;
}



void loop()
{

  if (readline(Serial.read(), buf, 80) > 0) {
    String string = String(buf);
    Serial.println(string);
    if (string.startsWith("red"))
    {
      Serial.println(string.substring(3).toInt());
      stmp[0] = string.substring(3).toInt();
    }
    else if (string.startsWith("green"))
    {
      Serial.println(string.substring(5).toInt());
      stmp[1] = string.substring(5).toInt();
    }
    else if (string.startsWith("blue"))
    {
      Serial.println(string.substring(4).toInt());
      stmp[2] = string.substring(4).toInt();
    }
    else if (string.startsWith("on"))
    {
      Serial.println(string.substring(2).toInt());
      stmp[3] = string.substring(2).toInt();
    }
    else if (string.startsWith("blink"))
    {
      Serial.println(string.substring(5).toInt());
      stmp[4] = string.substring(5).toInt();
    }
    else if (string.startsWith("send"))
    {
      Serial.println("OK");
      CAN.sendMsgBuf(0x43, 0, 8, stmp);
      usenino = false;
    }
    else if (string.startsWith("nino"))
    {
      Serial.println("OK");
      usenino = true;
    }
        else if (string.startsWith("offnino"))
    {
      Serial.println("OK");
      usenino = false;
      for (int i = 0; i < 8; i++)
          stmp[i] = 0;
      CAN.sendMsgBuf(0x43, 0, 8, stmp);
    }
  }
  
  if (usenino)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - blinkMillis > 500) {
      blinkMillis = currentMillis;
      if (!redblue)
      {
        for (int i = 0; i < 8; i++)
          stmp[i] = 0;
        stmp[2] = 0;
        stmp[0] = 50;
        CAN.sendMsgBuf(0x43, 0, 8, stmp);
        redblue = true;
      }
      else
      {
        for (int i = 0; i < 8; i++)
          stmp[i] = 0;
        stmp[2] = 50;
        stmp[0] = 0;
        CAN.sendMsgBuf(0x43, 0, 8, stmp);
        redblue = false;
      }

    }
  }
  
}
