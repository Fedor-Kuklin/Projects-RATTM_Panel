#include "ScreenAnalogSettings.h"
#include "../utils.h"

extern FutabaCyrillicLCD lcd;

#include <avr/pgmspace.h>

static const char analogType_0[] PROGMEM = "Температура";
static const char analogType_1[] PROGMEM = "Давление";
const char *const analogTypes[] PROGMEM = {analogType_0, analogType_1};
const uint16_t ADC_4MA = 4500;
const uint16_t ADC_20MA = 35000;

ScreenAnalogSettings screenAnalogSettings;

void ScreenAnalogSettings::update() {}

// Рендер по умолчанию (без индекса). Обычно используется render(index).
void ScreenAnalogSettings::render() {
}

void ScreenAnalogSettings::render(uint8_t analogIndex) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printRus("Настройки аналога");
    
    AnalogInputConfig* config = &gState.analogConfigs[analogIndex];
    
    // Тип датчика
    lcd.setCursor(0, 1);
    lcd.print(gState.analogSettingsSelected == 0 ? ">" : " ");
        lcd.printRus("Тип: ");
        {
            char tmp[24];
            readProgmemTableString(analogTypes, config->type, tmp, sizeof(tmp));
            lcd.printRus(tmp);
        }
    
    // Предел датчика (min/max, масштабировано на 100)
    lcd.setCursor(0, 2);
    lcd.print(gState.analogSettingsSelected == 1 ? ">" : " ");
    char buf[32];
    // config->minValueScaled/maxValueScaled уже масштабированы на 100
    int minWhole = config->minValueScaled / 100;
    int minFrac = abs(config->minValueScaled % 100);
    int maxWhole = config->maxValueScaled / 100;
    int maxFrac = abs(config->maxValueScaled % 100);
    snprintf(buf, sizeof(buf), "Предел: %d.%02d-%d.%02d", minWhole, minFrac, maxWhole, maxFrac);
    lcd.printRus(buf);

    // Ток (целочисленные вычисления, масштабировано на 100)
    lcd.setCursor(0, 3);
    int32_t raw = gState.analogInputs[analogIndex];
    int32_t range = (int32_t)ADC_20MA - (int32_t)ADC_4MA;
    int32_t currentScaled100 = 400; // 4.00 мА -> 400
    if (range > 0) {
    // (raw - ADC_4MA) / range * 1600 + 400  (в целых, масштабировано на 100)
        currentScaled100 = ((raw - (int32_t)ADC_4MA) * 1600 + range/2) / range + 400;
    }
    int curWhole = currentScaled100 / 100;
    int curFrac = abs(currentScaled100 % 100);
    snprintf(buf, sizeof(buf), "Ток: %d.%02d мА", curWhole, curFrac);
    lcd.printRus(buf);
    
    // Показывать
    lcd.setCursor(0, 4);
    lcd.print(gState.analogSettingsSelected == 2 ? ">" : " ");
    snprintf(buf, sizeof(buf), "Показывать: %s", config->show ? "Да" : "Нет");
    lcd.printRus(buf);
}

void ScreenAnalogSettings::handleKey(char key, uint8_t analogIndex) {
    AnalogInputConfig* config = &gState.analogConfigs[analogIndex];
    
        if (!(gState.flags & FLAG_ANALOG_EDIT)) {
        switch (key) {
            case 'U':
                if (gState.analogSettingsSelected > 0) gState.analogSettingsSelected--;
                break;
            case 'D':
                if (gState.analogSettingsSelected < 2) gState.analogSettingsSelected++;
                break;
            case 'E':
                    gState.flags |= FLAG_ANALOG_EDIT;
                break;
            case 'W':
                // Выход — обработка происходит в main.cpp
                break;
        }
    } else {
        switch (key) {
            case 'U':
                if (gState.analogSettingsSelected == 0) {
                    config->type = (config->type + 1) % 2;
                } else if (gState.analogSettingsSelected == 1) {
                    // min/max масштабированы на 100
                    if (gState.analogSettingsEditField == 0) {
                        config->minValueScaled += 10; // +0.1
                    } else {
                        config->maxValueScaled += 10; // +0.1
                    }
                } else if (gState.analogSettingsSelected == 2) {
                    config->show = !config->show;
                }
                break;
            case 'D':
                if (gState.analogSettingsSelected == 0) {
                    config->type = (config->type == 0) ? 1 : 0;
                } else if (gState.analogSettingsSelected == 1) {
                    if (gState.analogSettingsEditField == 0) {
                        int v = config->minValueScaled - 10; // -0.1
                        config->minValueScaled = (v < 0) ? 0 : v;
                    } else {
                        int v = config->maxValueScaled - 10; // -0.1
                        config->maxValueScaled = (v < 0) ? 0 : v;
                    }
                } else if (gState.analogSettingsSelected == 2) {
                    config->show = !config->show;
                }
                break;
            case 'E':
                if (gState.analogSettingsSelected == 1) {
                    gState.analogSettingsEditField = (gState.analogSettingsEditField + 1) % 2;
                } else {
                        gState.flags &= ~FLAG_ANALOG_EDIT;
                }
                break;
            case 'W':
                    gState.flags &= ~FLAG_ANALOG_EDIT;
                break;
        }
    }
    render(analogIndex);
}

void ScreenAnalogSettings::enter() {
    // При входе в экран отрисовать текущий индекс аналогового входа
    render(gState.analogSettingsIndex);
}

void ScreenAnalogSettings::exit() {}

void ScreenAnalogSettings::save() {}
void ScreenAnalogSettings::load() {}

// Устаревшие обёртки удалены; используйте screenAnalogSettings.render(index)
// и screenAnalogSettings.handleKey(key, index) напрямую