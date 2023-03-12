#pragma once
//
//    FILE: AdvKeyPad.h
//  AUTHOR: Walter Kvapil
// VERSION: 0.1.0
// PURPOSE: Arduino advanced keypad library for 4x4 KeyPad with PCF8574 I2C-Adapter
//     URL: https://github.com/WK-Software56/AdvKeyPad


#include "Arduino.h"
#include "Wire.h"


#define KP__LIB_VERSION    (F("0.1.0"))

#define KP__NOKEY          16
#define KP__FAIL           17

#define KP_PRESS		1
#define KP_DOWN			2
#define KP_SENT			4
#define KP_ISSHIFT		8
#define KP_RESET		8

#define KP__DELAY		1
#define KP__REPEAT		2

#define KP__REVERSE			true


#define FIFO_BUFSIZE		10

class AdvKeyPad
{
public:
  AdvKeyPad(const uint8_t deviceAddress, TwoWire *wire = &Wire);

#if defined(ESP8266) || defined(ESP32)
  bool    begin(uint8_t sda, uint8_t scl);
#endif
  bool    begin();

  //  get raw key's 0..15
  uint8_t getKey();
  uint8_t getLastKey();

  bool    isPressed();
  bool    isConnected();

  //  get 'translated' keys
  //  user must load KeyMap, there is no check.
  uint8_t getChar();
  uint8_t getLastChar();
  void    loadKeyMap(char * keyMap);   //  char[19]

  void	  setKeyPadDebounce(uint8_t debtime);
  uint8_t getKeyPadDebounce();
  void	  setKeyPadRepeat(int delay, int repeat);
  void	  setKeyPadShift(uint8_t key, uint8_t offset);

protected:
  uint8_t _i2cAddr;
  uint8_t _lastKey, _trigKey;
  int _repDelay;
  int _repRepeat;
  uint8_t _repStatus;
  unsigned long _repTimer;
  uint8_t _debounce;
  unsigned long _trigger[16];
  uint8_t _status[16];
  uint8_t _shiftOffset[16];
  uint8_t _getKeyPad();
  uint8_t _read(uint8_t mask);
  uint8_t _buffer[FIFO_BUFSIZE];
  int _inpos;
  int _opos;
  void _fifo_put(uint8_t key);
  uint8_t _fifo_get();
  bool _fifo_isempty();

  TwoWire* _wire;
  char *  _keyMap = NULL;
};
// -- END OF FILE --
