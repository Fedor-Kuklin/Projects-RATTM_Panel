#include "UtilAnalog.h"
#include "ScreenAnalogSettings.h"

uint16_t analogInputsRaw[5] = {0, 0, 0, 0, 0}; // Сырые значения АЦП

extern const uint16_t ADC_4MA;
extern const uint16_t ADC_20MA;

// Преобразует сырое значение АЦП в физическое значение, масштабированное на 100
// Параметры:
//  - raw: значение с АЦП
//  - index: индекс конфигурации датчика в gState.analogConfigs
// Возвращает: значение в формате (physical * 100) в диапазоне int16_t
int16_t convertAnalogToPhysical(uint16_t raw, uint8_t index) {
  int32_t range = (int32_t)ADC_20MA - (int32_t)ADC_4MA;
  // По умолчанию 4.00 мА -> 400 (масштабировано на 100)
  int32_t currentScaled100 = 400;
  if (range > 0) {
    // (raw - ADC_4MA) / range * 1600 + 400  (целочисленные вычисления)
    currentScaled100 = ((int32_t)(raw - ADC_4MA) * 1600 + range/2) / range + 400;
  }

   // phys_scaled100 = (current_mA - 4.00) / 16.00 * (max-min) + min
   // currentScaled100 и min/max оба масштабированы на 100
  int32_t deltaCfg = (int32_t)gState.analogConfigs[index].maxValueScaled - (int32_t)gState.analogConfigs[index].minValueScaled; // масштабировано на 100
  int32_t numerator = (currentScaled100 - 400) * deltaCfg; // масштабировано на 100*100
  // делим на 1600 (16.00*100) чтобы получить результат, масштабированный на 100
  int32_t physScaled100 = 0;
  if (deltaCfg != 0) {
    // округление при делении
    if (numerator >= 0) physScaled100 = (numerator + 800) / 1600 + gState.analogConfigs[index].minValueScaled;
    else physScaled100 = (numerator - 800) / 1600 + gState.analogConfigs[index].minValueScaled;
  } else {
    physScaled100 = gState.analogConfigs[index].minValueScaled;
  }

  // Ограничить результат в пределах int16
  if (physScaled100 > 32767) physScaled100 = 32767;
  if (physScaled100 < -32768) physScaled100 = -32768;
  return (int16_t)physScaled100;
}

// Обновляет массив gState.analogValuesScaled, вызывая преобразование для каждого входа
void updateAnalogValues() {
  for (int i = 0; i < 5; i++) {
    gState.analogValuesScaled[i] = convertAnalogToPhysical(analogInputsRaw[i], i);
  }
}