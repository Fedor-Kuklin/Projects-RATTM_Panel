#include "UtilAnalog.h"
#include "ScreenAnalogSettings.h"

uint16_t analogInputsRaw[5] = {0, 0, 0, 0, 0}; // Сырые значения АЦП

extern const uint16_t ADC_4MA;
extern const uint16_t ADC_20MA;

int16_t convertAnalogToPhysical(uint16_t raw, uint8_t index) {
  // Конвертация из АЦП в мА (целочисленно, scaled by 100)
  int32_t range = (int32_t)ADC_20MA - (int32_t)ADC_4MA;
  int32_t currentScaled100 = 400; // default 4.00 mA
  if (range > 0) {
    currentScaled100 = ((int32_t)(raw - ADC_4MA) * 1600 + range/2) / range + 400; // (raw-ADC_4MA)/range *1600 +400
  }

  // phys_scaled100 = (current_mA - 4.00) / 16.00 * (max-min) + min
  // currentScaled100 and min/max are both scaled by 100
  int32_t deltaCfg = (int32_t)gState.analogConfigs[index].maxValueScaled - (int32_t)gState.analogConfigs[index].minValueScaled; // scaled by 100
  int32_t numerator = (currentScaled100 - 400) * deltaCfg; // scaled by 100*100
  // divide by 1600 (i.e., 16.00*100) to get scaled by 100
  int32_t physScaled100 = 0;
  if (deltaCfg != 0) {
    // add rounding
    if (numerator >= 0) physScaled100 = (numerator + 800) / 1600 + gState.analogConfigs[index].minValueScaled;
    else physScaled100 = (numerator - 800) / 1600 + gState.analogConfigs[index].minValueScaled;
  } else {
    physScaled100 = gState.analogConfigs[index].minValueScaled;
  }

  if (physScaled100 > 32767) physScaled100 = 32767;
  if (physScaled100 < -32768) physScaled100 = -32768;
  return (int16_t)physScaled100;
}

void updateAnalogValues() {
  for (int i = 0; i < 5; i++) {
    gState.analogValuesScaled[i] = convertAnalogToPhysical(analogInputsRaw[i], i);
  }
}