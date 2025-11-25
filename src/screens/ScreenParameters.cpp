#include "ScreenParameters.h"
#include "../utils.h"

extern FutabaCyrillicLCD lcd;

ScreenParameters screenParameters;

void ScreenParameters::enter() {}
void ScreenParameters::exit() {}
void ScreenParameters::update() {}

void ScreenParameters::render() {
    lcd.clear();
    
    // Заголовок
    lcd.setCursor(0, 0);
    lcd.printRus("Экран параметров");
    
    // Подсчитываем сколько параметров показывать
    uint8_t visibleParams = 0;
    for (uint8_t i = 0; i < 5; i++) {
        if (gState.analogConfigs[i].show) {
            visibleParams++;
        }
    }
    
    const int visibleCount = 3; // 4 строки - 1 заголовок
    
    // Ограничиваем скролл
    int maxScroll = visibleParams - visibleCount;
    if (maxScroll < 0) maxScroll = 0;
    if (gState.paramsScroll > maxScroll) gState.paramsScroll = maxScroll;
    if (gState.paramsScroll < 0) gState.paramsScroll = 0;
    
    // Отображаем параметры с учетом скролла
    uint8_t row = 1;
    uint8_t displayed = 0;
    
    for (uint8_t i = 0; i < 5 && row < 4; i++) {
        if (gState.analogConfigs[i].show) {
            // Пропускаем скроллируемые элементы
            if (displayed < gState.paramsScroll) {
                displayed++;
                continue;
            }
            
            char *buf = g_scratch;
            int16_t scaled = gState.analogValuesScaled[i];
            int whole = scaled / 100;
            int frac = abs(scaled % 100);
            if (gState.analogConfigs[i].type == 0) {
                // Температура — показываем с двумя знаками после запятой
                snprintf(buf, sizeof(g_scratch), "T%d: %d.%02dC", i + 1, whole, frac);
            } else {
                // Давление — два знака после запятой
                snprintf(buf, sizeof(g_scratch), "P%d: %d.%02dкПа", i + 1, whole, frac);
            }

            lcd.setCursor(0, row);
            lcd.printRus(buf);
            row++;
            displayed++;
        }
    }
    
    // Показываем индикатор скролла если есть больше параметров
    if (visibleParams > visibleCount) {
        lcd.setCursor(19, 3);
        if (gState.paramsScroll > 0 && gState.paramsScroll < maxScroll) {
            lcd.writeChar(0x7E); // Двойная стрелка
        } else if (gState.paramsScroll > 0) {
            lcd.writeChar(0x1E); // Стрелка вверх
        } else if (gState.paramsScroll < maxScroll) {
            lcd.writeChar(0x1F); // Стрелка вниз
        }
    }
}

void ScreenParameters::handleKey(char key) {
    // Подсчитываем сколько параметров показывать
    uint8_t visibleParams = 0;
    for (uint8_t i = 0; i < 5; i++) {
        if (gState.analogConfigs[i].show) {
            visibleParams++;
        }
    }
    
    const int visibleCount = 3;
    int maxScroll = visibleParams - visibleCount;
    if (maxScroll < 0) maxScroll = 0;
    
    switch (key) {
        case 'U':
            if (gState.paramsScroll > 0) {
                gState.paramsScroll--;
            }
            break;
            
        case 'D':
            if (gState.paramsScroll < maxScroll) {
                gState.paramsScroll++;
            }
            break;
            
        case 'E':
            // Обновление данных по требованию
            loadAnalogInputsFromRegisters();
            break;
            
        case 'W':
            requestState(MAIN_MENU);
            break;
    }
    
    // Если запросили смену экрана — не рендерим текущий экран дополнительно
    if (gState.nextState < 0) {
        render();
    }
}

void ScreenParameters::save() {}
void ScreenParameters::load() {}

// Устаревшие обёртки удалены; используйте screenParameters.render() и
// screenParameters.handleKey(key) напрямую