//
//    FILE: AdvKeyPad.cpp
//  AUTHOR: Walter Kvapil
// VERSION: 1.0.0
// PURPOSE: Arduino advanced keypad library for 4x4 KeyPad with PCF8574 I2C-Adapter
//     URL: https://github.com/WK-Software56/AdvKeyPad
//
// HISTORY: see changelog.md

#include "AdvKeyPad.h"

AdvKeyPad::AdvKeyPad(const uint8_t devAddr, TwoWire *wire)
{
  _lastKey = KP__NOKEY;
  _i2cAddr = devAddr;
  _wire    = wire;
  _debounce = 0;
  _repDelay = 0;
  _repRepeat = 0;
  for (int i=0;i<16;i++) {
    _trigger[i] = 0;
    _status[i] = 0;
  }
  _inpos=0;
  _opos=0;
  _repStatus=0;
}

#if defined(ESP8266) || defined(ESP32)
bool AdvKeyPad::begin(uint8_t sda, uint8_t scl)
{
  _wire->begin(sda, scl);
  //  enable interrupts
  _read(0xF0);
  return isConnected();
}
#endif

bool AdvKeyPad::begin()
{
  _wire->begin();
  //  enable interrupts
  _read(0xF0);
  return isConnected();
}

uint8_t AdvKeyPad::getLastKey()   
{ 
  return _lastKey;
};

//  to check "press any key"
bool AdvKeyPad::isPressed()
{
  uint8_t a = _read(0xF0);
  if (a == 0xFF) return false;
  return (a != 0xF0);
}

bool AdvKeyPad::isConnected()
{
  _wire->beginTransmission(_i2cAddr);
  return (_wire->endTransmission() == 0);
}

uint8_t AdvKeyPad::getKey()
{
  return _getKeyPad();
}

uint8_t AdvKeyPad::getChar()
{ 
  return _keyMap[getKey()]; 
}


uint8_t AdvKeyPad::getLastChar()
{ 
  return _keyMap[_lastKey]; 
}

void AdvKeyPad::loadKeyMap(char * keyMap)
{
  strcpy(_keyMap, keyMap);
}

void AdvKeyPad::setKeyPadDebounce(uint8_t debtime)
{
  _debounce = debtime;
}

uint8_t AdvKeyPad::getKeyPadDebounce()
{
  return _debounce;
}

void AdvKeyPad::setKeyPadRepeat(int delay, int repeat)
{
  if(delay>0) {
    _repDelay=delay;
    if(repeat==0) _repRepeat=delay/4;
    else _repRepeat=repeat;
  }
}

void AdvKeyPad::setKeyPadShift(uint8_t key, uint8_t offset)  // Experimental for future releases
{
  _status[key]|=KP_ISSHIFT;
  _shiftOffset[key]=offset;
}


//
//  PROTECTED Functions
//
uint8_t AdvKeyPad::_read(uint8_t mask)
{
  //  
  yield();

  _wire->beginTransmission(_i2cAddr);
  _wire->write(mask);
  if (_wire->endTransmission() != 0)
  {
    //  set communication error
    return 0xFF;
  }
  _wire->requestFrom(_i2cAddr, (uint8_t)1);
  return _wire->read();
}

uint8_t AdvKeyPad::_getKeyPad()
{
  uint8_t  rowmask;
  uint8_t  k;

  // Check for repeater
  //Serial.println("Lastkey="+String(_lastKey)+" status: "+String(_status[_lastKey]));
  if((_repStatus>0) && ((_status[_lastKey]&KP_DOWN)>0)) {
    //Serial.print("Repeating ");
    if(_repStatus==KP__DELAY) {
        //Serial.println("DELAY");
      //Serial.println("Millis="+String(millis())+" RepTimer="+String(_repTimer)+" RepDelay="+String(_repDelay));
      if(millis()>(_repTimer+_repDelay)) {
        _fifo_put(_lastKey);
        _repTimer=millis();
        _repStatus=KP__REPEAT;
      }
    }
    else if (_repStatus==KP__REPEAT) {
        //Serial.println("REPEAT");
      if(millis()>(_repTimer+_repRepeat)) {
        _fifo_put(_lastKey);
        _repTimer=millis();
      }
    }
  }
  else _repStatus=0;

  // Checking if key pressed
  //
  for (int row=0; row<4; row++) {
    if(KP__REVERSE) rowmask=0xff-(1<<7-row);
    else  rowmask=0xff-(1<<row+4);
    //Serial.print("Rowmask=");
    //Serial.print(rowmask,HEX);
    //Serial.print("  Read Key=");
    _wire->beginTransmission(_i2cAddr);
    _wire->write(rowmask);  // set Row Mask
    _wire->endTransmission();
    _wire->requestFrom(_i2cAddr,(uint8_t)1);
    uint8_t rkey=_wire->read();  // Read keypad Row
    //Serial.print(rkey,HEX);
    //Serial.print(" -> ");
    rkey^=rowmask;  // Filter Changed Keys
    //Serial.println(rkey,HEX);
    for (int col=0; col<4; col++) {
      if(KP__REVERSE)  k=rkey&(1<<3-col);
      else  k=rkey&(1<<col);
      int keynum=row*4+col;  // Current KeyNumber
      if (k==0) {  // Key not pressed
        if ((_status[keynum]&KP_DOWN) > 0) {  //Not in debounce
          _status[keynum]&=KP_RESET;  // Reset all Flags for Key
        }
      }
      else {        // Key pressed
        //Serial.print("Key pressed:  ");
        //Serial.println(keynum);
        //Serial.print("Status=");
        //Serial.println(_status[keynum]&KP_DOWN,HEX);
        if ((_status[keynum]&KP_DOWN) > 0) {  // Key already down
          //Serial.print("Key is down ");
          //Serial.println(keynum,HEX);
          if ((_status[keynum]&KP_SENT) == 0) {  // Key not sent
            _fifo_put(keynum);  // Put key into Buffer
            //Serial.print("Sending Key ");
            //Serial.println(_status[keynum],HEX);
            _lastKey=keynum;
            _status[keynum]|=KP_SENT;
            if(_repDelay>0) {
              _repTimer=millis();  // Set trigger for Repeater
              _repStatus=KP__DELAY;
            }
          }
        }
        else {
          if((_status[keynum]&KP_PRESS) > 0) {
            if(millis()>_trigger[keynum]+_debounce) {  //Debounce Time Elapsed
              //Serial.print("Debounce finish, Key down ");
              //Serial.println(_status[keynum],HEX);
              _status[keynum]|=KP_DOWN;
            }
          }
          else {        // Start Debouncing
            //Serial.print("Start Debounce Key ");
            //Serial.println(_status[keynum],HEX);
            _trigger[keynum]=millis();
            _status[keynum]|=KP_PRESS;    // Set Debounce Status
          }
        }
      }
    }  // End of col loop
  }  // End of Row loop

  return _fifo_get();
}

void AdvKeyPad::_fifo_put(uint8_t key) {
  _buffer[_inpos++]=key;
  if (_inpos==FIFO_BUFSIZE) _inpos=0;
}

uint8_t AdvKeyPad::_fifo_get() {
uint8_t  key;  
  if(_inpos==_opos) {
    _inpos=_opos=0;
    key=KP__NOKEY;
  }
  else {
    key=_buffer[_opos++];
    if(_opos==FIFO_BUFSIZE) _opos=0;
  }
  return key;
}

bool AdvKeyPad::_fifo_isempty() {
  return (_inpos==_opos);
}

// -- END OF FILE --

