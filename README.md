[![Arduino CI](https://github.com/WK-Software56/AdvKeyPad/workflows/Arduino_CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![arduino-lint-action](https://github.com/WK-Software56/AdvKeyPad/actions/workflows/arduino-lint-action.yml/badge.svg)](https://github.com/WK-Software56/AdvKeyPad/.github/workflows/arduino-lint-action.yml)
# AdvKeyPad
Arduino advanced Keypad-Library for 4x4 Keypads with PCF8574 I2C Interface
## Description
The Advanced KeyPad Library is used to connect a 4x4 Kepad by using a PFC8574 I2C-Interface.
The Programming Interface is based on the I2CKeyPad library by Rob Tillaart, but with a complete
new Key-Scanning Module.
The goal for this Library was to handle the KeyPad like a normal Keyboard including debounce and repeating keys.

The PCF8574 is connected between the processor and the 4x4 keypad. See the conceptual schema below. 

          PROC             PCF8574              KEYPAD
        +--------+        +--------+          +--------+
        |        |        |       0|----------|R       |
        |    SDA |--------|       1|----------|O       |
        |    SCL |--------|       2|----------|W       |
        |        |        |       3|----------|S       |
        |        |        |        |          |        |
        |        |        |       4|----------|C       |
        |        |        |       5|----------|O       |
        |        |        |       6|----------|L       |
        |        |        |       7|----------|S       |
        +--------+        +--------+          +--------+ 

## Interface

    AdvKeyPad(const uint8_t devAddr, TwoWire *wire = &Wire) 
       The constructor sets the device address and optionally allows to selects the I2C bus to use.
    bool begin() The return value shows if the PCF8574 with the given address is connected properly.
    bool begin(uint8_t sda, uint8_t scl) for ESP32. The return value shows if the PCF8574 with the given address is connected properly.
    bool isConnected() returns false if the PCF8574 cannot be connected to.
    uint8_t getKey() Returns default 0..15 for regular keys, Returns 16 if no key is pressed and 17 in case of an error.
    uint8_t getLastKey() Returns the last valid key pressed 0..15. Initially it will return 16 (noKey).
    
## KeyMap functions

    loadKeyMap() must be called before getChar() and getLastChar()!

    char getChar() returns the char corresponding to mapped key pressed.
    char getLastChar() returns the last char pressed.
    bool loadKeyMap(char * keyMap) keyMap should point to a (global) char array of length 19. 
      This array maps index 0..15 on a char and index [16] maps to I2CKEYPAD_NOKEY (typical 'N') and index [17] maps I2CKEYPAD_FAIL (typical 'F'). index 18 is the null char.

After the keypad.begin() the sketch calls the keyPad.getKey() to read values from the keypad.

    If no key is pressed KP__NOKEY code (16) is returned.
    If the read value is not valid, KP__FAIL code (17) is returned.
    Otherwise a number 0..15 is returned.

# Future

    update documentation
    Shift-like Function where you can define The Keys A-D to shifting keys like Shift/Ctrl/Alt on normal keyboards
    This enables you to expand the possible Input-Values.
