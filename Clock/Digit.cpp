#include "Digit.h"

Digit::Digit(){
  _num = -1;  
}

Digit::Digit(char num){
  _num = num;
}

char Digit::getNumber(){
  return _num;
}

void Digit::setNumber(char num){
  _num = num;
  _converted = false;
}

bool Digit::isHexRepresentation(){
  return _hex;  
}

void Digit::setHex(bool hex){
  _hex = hex;
  _converted = false;
}

void Digit::custom(char converted){
  _converted = true;
  _convert = converted;
}

char Digit::convert(){

  if(_converted) return _convert;
  
  char digitRep = 0;


  //0 = 00000000b digitRep = 01110111b;
  //1 = 00000001b digitRep = 00010010b;
  //2 = 00000010b digitRep = 01101011b;
  //3 = 00000011b digitRep = 01011011b;
  //4 = 00000100b digitRep = 00011110b;
  //5 = 00000101b digitRep = 01011101b;
  //6 = 00000110b digitRep = 01111101b;
  //7 = 00000111b digitRep = 00010111b;
  //8 = 00001000b digitRep = 01111111b;
  //9 = 00001001b digitRep = 00011111b

  switch(_num){
    
    case 0:
      digitRep = 0x77;//B01110111;
      break;
    case 1:
      digitRep = 0x12;//B00010010;
      break;
    case 2:
      digitRep = 0x6b;//01101011b;
      break;
    case 3:
      digitRep = 0x5b;//01011011b;
      break;
    case 4:
      digitRep = 0x1e;//00011110b;
      break;
    case 5:
      digitRep = 0x5d;//01011101b;
      break;
    case 6:
      digitRep = 0x7d;//01111101b;
      break;
    case 7:
      digitRep = 0x17;//00010111b;
      break;
    case 8:
      digitRep = 0x7f;//01111111b;
      break;
    case 9:
      digitRep = 0x1f;//00011111b;
      break;
    default:
      digitRep = 0x00;//B00000000;
      break;

  }
  
  _convert = digitRep;
  _converted = true;
  
  return digitRep;

  
}

bool Digit::converted(){
  return _converted;
}
