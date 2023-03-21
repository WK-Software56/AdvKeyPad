//
//    FILE: Advkeypad_demo.ino
//  AUTHOR: Walter Kvapil
// PURPOSE: demo
//     URL: https://github.com/WK-Software56/AdvKeyPad
//
//  PCF8574
//    pin p0-p3 rows
//    pin p4-p7 columns
//  4x4 or smaller keypad.


#include "Wire.h"
#include "AdvKeyPad.h"

const uint8_t KEYPAD_ADDRESS = 0x20;

AdvKeyPad keyPad(KEYPAD_ADDRESS);


void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }
}


void loop()
{
    uint8_t key;
    key=keyPad.read();
    if(key != KP__NOKEY) {
      Serial.print(key);
      Serial.print("\t");
      Serial.println(keyPad.getLastChar());
    }
  }
}


// -- END OF FILE --
