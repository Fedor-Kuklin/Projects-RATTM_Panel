/*
 * Created: 01.07.2024 13:00:39
 *  Author: Admin
 */ 


// ExternalKeyPad.h
#ifndef ExternalKeyPad_H
#define ExternalKeyPad_H

#include <Keypad.h>

// Объявляем, что в .cpp есть объект customKeypad
extern Keypad customKeypad;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

// Упрощённый (плоский) массив для makeKeymap
extern char hexaKeys[ROWS * COLS];
extern byte rowPins[ROWS];
extern byte colPins[COLS];

#endif // ExternalKeyPad_H