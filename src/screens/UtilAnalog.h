#pragma once
#include <stdint.h>
#include "GlobalState.h"

extern uint16_t analogInputsRaw[5];

// Возвращает значение, масштабированное на 100 (physical * 100)
int16_t convertAnalogToPhysical(uint16_t raw, uint8_t index);
void updateAnalogValues();