#include "ScreenAnalogInputsMenu.h"
#include "../utils.h"

extern FutabaCyrillicLCD lcd;
extern Menu analogInputsMenu;

ScreenAnalogInputsMenu screenAnalogInputsMenu;

void ScreenAnalogInputsMenu::enter() {}
void ScreenAnalogInputsMenu::exit() {}
void ScreenAnalogInputsMenu::update() {}

void ScreenAnalogInputsMenu::render() {
    lcd.clear();
    
    // Заголовок
    int titleLen = utf8_strlen(analogInputsMenu.title);
    int titlePad = (20 - titleLen) / 2;
    lcd.setCursor(titlePad > 0 ? titlePad : 0, 0);
    lcd.printRus(analogInputsMenu.title);
    
    // Пункты меню со значениями
    for (int i = 0; i < 3; i++) {
        int itemIdx = i + gState.analogMenuState.scroll;
        lcd.setCursor(0, i + 1);
        
        if (itemIdx < analogInputsMenu.length) {
            lcd.writeChar((itemIdx == gState.analogMenuState.selectedIndex) ? 0x10 : ' ');
            
            char *buf = g_scratch;
            int16_t scaled = gState.analogValuesScaled[itemIdx];
            int whole = scaled / 100;
            int frac = abs(scaled % 100);
            snprintf(buf, sizeof(g_scratch), "Аналог %d: %d.%02d", itemIdx + 1, whole, frac);
            lcd.printRus(buf);
        }
    }
}

void ScreenAnalogInputsMenu::save() {}
void ScreenAnalogInputsMenu::load() {}

// Удалён устаревший обёртывающий show-функция; используйте screenAnalogInputsMenu.render() напрямую

void ScreenAnalogInputsMenu::handleKey(char key) {
    switch (key) {
        case 'U':
            gState.analogMenuState.selectedIndex = (gState.analogMenuState.selectedIndex > 0) ? 
                    gState.analogMenuState.selectedIndex - 1 : analogInputsMenu.length - 1;
            break;
        case 'D':
            gState.analogMenuState.selectedIndex = (gState.analogMenuState.selectedIndex < analogInputsMenu.length - 1) ? 
                    gState.analogMenuState.selectedIndex + 1 : 0;
            break;
            case 'E':
                gState.analogSettingsIndex = gState.analogMenuState.selectedIndex;
                gState.analogSettingsSelected = 0;
                gState.flags &= ~FLAG_ANALOG_EDIT;
                requestState(ANALOG_SETTINGS_MENU);
                return;
            case 'W':
                requestState(MAIN_MENU);
                return;
    }
    updateMenuScroll(gState.analogMenuState, &analogInputsMenu);
    this->render();
}