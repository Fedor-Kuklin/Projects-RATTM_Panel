/*
 * ExternalKeyPad.cpp
 *
 * Created: 01.07.2024 13:01:53
 *  Author: Admin
 */ 

// ExternalKeyPad.cpp
#include "ExternalKeyPad.h"

// Определяем плоский массив ключей (16 элементов)
char hexaKeys[ROWS * COLS] = {
  '1','2','3','U',
  '4','5','6','D',
  '7','8','9','W',
  '0','.','E','-'
};
byte rowPins[ROWS] = {45, 46, 47, 48}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 