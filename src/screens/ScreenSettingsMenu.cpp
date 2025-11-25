#include "ScreenSettingsMenu.h"
#include "../utils.h"

extern FutabaCyrillicLCD lcd;
extern Menu settingsMenu;

ScreenSettingsMenu screenSettingsMenu;

void ScreenSettingsMenu::enter() {}
void ScreenSettingsMenu::exit() {}
void ScreenSettingsMenu::update() {}

void ScreenSettingsMenu::render() {
    lcd.clear();
    
    // Заголовок
    char tmpTitle[24];
    readProgmemString(settingsTitle, tmpTitle, sizeof(tmpTitle));
    int titleLen = utf8_strlen(tmpTitle);
    int titlePad = (20 - titleLen) / 2;
    lcd.setCursor(titlePad > 0 ? titlePad : 0, 0);
    lcd.printRus(tmpTitle);
    
    // Пункты меню
    for (int i = 0; i < 3; i++) {
        int itemIdx = i + gState.settingsMenuState.scroll;
        lcd.setCursor(0, i + 1);
        
        if (itemIdx < settingsMenu.length) {
            lcd.writeChar((itemIdx == gState.settingsMenuState.selectedIndex) ? 0x10 : ' ');
            char tmp[32];
            readProgmemTableString(settingsItems, itemIdx, tmp, sizeof(tmp));
            lcd.printRus(tmp);
        }
    }
}

void ScreenSettingsMenu::save() {}
void ScreenSettingsMenu::load() {}

// Удалён устаревший обёртывающий show-функция; используйте screenSettingsMenu.render() напрямую
// Удалён устаревший обёртывающий show-функция; используйте screenSettingsMenu.render() напрямую

void ScreenSettingsMenu::handleKey(char key) {
        switch (key) {
        case 'U':
            gState.settingsMenuState.selectedIndex = (gState.settingsMenuState.selectedIndex > 0) ? 
                    gState.settingsMenuState.selectedIndex - 1 : settingsMenu.length - 1;
            break;
        case 'D':
            gState.settingsMenuState.selectedIndex = (gState.settingsMenuState.selectedIndex < settingsMenu.length - 1) ? 
                    gState.settingsMenuState.selectedIndex + 1 : 0;
            break;
        case 'E':
            switch (gState.settingsMenuState.selectedIndex) {
                    case 0: // UART
                    loadUARTFromRegisters();
                    gState.uartMenuState = {0, 0};
                    gState.flags &= ~FLAG_UART_EDIT;
                    requestState(UART_MENU);
                    // основной цикл обработает вход/отрисовку меню UART
                    return;
                case 1: // ETH
                    loadEthFromRegisters();
                    gState.ethMenuState = {0, 0};
                    gState.ethEditOctet = 0;
                    gState.flags &= ~FLAG_ETH_EDIT;
                    gState.ethEditOctet = 0;
                    requestState(ETH_MENU);
            // основной цикл обработает вход/отрисовку меню ETH
                    return;
            }
            break;
            case 'W':
                requestState(MAIN_MENU);
                return;
        }

    updateMenuScroll(gState.settingsMenuState, &settingsMenu);
    // повторная перерисовка текущего экрана
    this->render();
}