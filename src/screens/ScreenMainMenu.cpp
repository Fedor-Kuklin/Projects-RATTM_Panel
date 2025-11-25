#include "ScreenMainMenu.h"
#include "../utils.h"

extern FutabaCyrillicLCD lcd;
extern Menu mainMenu;

ScreenMainMenu screenMainMenu;

void ScreenMainMenu::enter() {
    // при необходимости — запуск таймеров/сброс состояния
}

void ScreenMainMenu::exit() {
    // при необходимости — очистка состояния
}

void ScreenMainMenu::update() {
    // логика обновления экрана (обработку клавиш держим вне render)
}

void ScreenMainMenu::render() {
    lcd.clear();
    
    // Заголовок (строка в PROGMEM)
    char tmpTitle[24];
    readProgmemString(mainMenuTitle, tmpTitle, sizeof(tmpTitle));
    int titleLen = utf8_strlen(tmpTitle);
    int titlePad = (20 - titleLen) / 2;
    lcd.setCursor(titlePad > 0 ? titlePad : 0, 0);
    lcd.printRus(tmpTitle);
    
    // Пункты меню
    for (int i = 0; i < 3; i++) {
        int itemIdx = i + gState.mainMenuState.scroll;
        lcd.setCursor(0, i + 1);
        
        if (itemIdx < mainMenu.length) {
            lcd.writeChar((itemIdx == gState.mainMenuState.selectedIndex) ? 0x10 : ' ');
            char tmp[32];
            readProgmemTableString(mainMenuItems, itemIdx, tmp, sizeof(tmp));
            lcd.printRus(tmp);
        }
    }
}

void ScreenMainMenu::save() {
    // сохранить состояние при выходе, если нужно
}

void ScreenMainMenu::load() {
    // загрузить состояние при входе, если нужно
}

// Совместимая функция для существующего кода
// Удалён устаревший обёртывающий show-функция; используйте screenMainMenu.render() напрямую

void ScreenMainMenu::handleKey(char key) {
        // Переиспользуем логику из старой handleMainMenu
        switch (key) {
            case 'U':
                gState.mainMenuState.selectedIndex = (gState.mainMenuState.selectedIndex > 0) ? 
                        gState.mainMenuState.selectedIndex - 1 : mainMenu.length - 1;
                break;
            case 'D':
                gState.mainMenuState.selectedIndex = (gState.mainMenuState.selectedIndex < mainMenu.length - 1) ? 
                        gState.mainMenuState.selectedIndex + 1 : 0;
                break;
            case 'E':
                switch (gState.mainMenuState.selectedIndex) {
                                case 0: // Экран параметров
                                    requestState(PARAMETERS_SCREEN);
                                    return;
                                case 1: // Настройки
                                    requestState(ENTER_PASSWORD);
                                    return;
                                case 3: // Аналоговые входы
                                    loadAnalogInputsFromRegisters();
                                    gState.analogMenuState = {0, 0};
                                    requestState(ANALOG_INPUTS_MENU);
                                    return;
                                case 4: // Дата и время
                                    loadDateTimeFromRegisters();
                                    requestState(DATE_TIME_MENU);
                                    return;
                }
                break;
            case 'W':
                // Уже в главном меню - ничего не делаем
                break;
        }

        updateMenuScroll(gState.mainMenuState, &mainMenu);
        render();
}