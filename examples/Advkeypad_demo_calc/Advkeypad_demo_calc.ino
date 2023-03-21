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

// A little  calculator for Integer Numbers
// Using special Keymap:  A=/, B=*, C=-, D=+, #=SUM, *=CLR

#include "Wire.h"
#include "AdvKeyPad.h"
#include <string.h>

const uint8_t KEYPAD_ADDRESS = 0x20;

AdvKeyPad keyPad(KEYPAD_ADDRESS);

int  opstack[10],valstack[10]; // Stack for Operands and Values to perform point calculation before line calculation

//int opval; // Number  entered
//char operand; // Operand entered
int sum; // Calculation Result
char instr[20]; // Input String
uint8_t vsp,osp,icnt; // Stack Pointers for Values and operands

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
  Serial.println();
  Serial.println("C A L C U L A T O R");
  Serial.println();
  keyPad.loadKeyMap("123/456*789-C0=+NF"); // Use special KeyMap
  vsp=osp=icnt=0;
  sum=0;
  
}


void loop()
{
  char key;
  
  key=keyPad.getChar();
  if(key != 'N') {
    Serial.print(key);
    if(key>='0' and key <='9') {   // A numeric Value entered
      instr[icnt++]=key;  // Add it to instr
      instr[icnt]='\0';  // Terminate String
    }
    else {
      switch(key) {
        case '+':
            push(instr);
            opush(key);
            calculate();
            break;
        case '-':
            push(instr);
            opush(key);
            calculate();
            break;
        case '*':
            push(instr);
            opush(key);
            calculate();
            break;
        case '/':
            push(instr);
            opush(key);
            calculate();
            break;
        case 'C':
            Serial.println();
            Serial.println("All Clear");
            vsp=osp=icnt=0;
            sum=0;
            break;
        case '=':
            push(instr);
            opush(key);
            while(osp>1) calculate();
            Serial.println();
            Serial.print("**** ");
            Serial.println(sum);
            vsp=osp=icnt=0;
            break;
      }
      icnt=0;
      instr[0]='\0';
    }
  }
}

void push(char * istr) {
  int i=atoi(istr);
  if(strlen(istr)==0) i=sum;
  valstack[vsp++]=i;
}

void push(int val) {
  valstack[vsp++]=val;
}

void opush(char op) {
  opstack[osp++]=op;
}

int pop() {
  return(valstack[--vsp]);
}

char opop() {
  return(opstack[--osp]);
}

void calculate() {
  while(osp>1){
    char op1=opop();
    char op2=opop();
    if(op1=='*' || op1=='/') {
      if(op2=='+' || op2=='-') {
        opush(op2);
        opush(op1);
        return;
      }
    }
    int v1=pop();
    int v2=pop();
    switch(op2) {
      case '+':
        sum=v1+v2;
        break;
      case '-':
        sum=v2-v1;
        break;
      case '*':
        sum=v2*v1;
        break;
      case '/':
        sum=v2/v1;
        break;
    }
    push(sum);
    opush(op1);
  }
}

// -- END OF FILE --
