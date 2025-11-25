#include "ScreenEnterPassword.h"
#include "../utils.h"
#include <string.h>
#include <stdio.h>

extern FutabaCyrillicLCD lcd;

ScreenEnterPassword screenEnterPassword;

void ScreenEnterPassword::enter() {
    len = 0;
    memset(input, 0, sizeof(input));
}
void ScreenEnterPassword::exit() {}
void ScreenEnterPassword::update() {}
void ScreenEnterPassword::load() { enter(); }

void ScreenEnterPassword::render() {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.printRus("Ввести пароль");
    lcd.setCursor(6, 2);
    for (int i = 0; i < 5; i++) {
        if (i < len) lcd.writeChar(0x2A);
        else lcd.writeChar(0x2D);
    }
    lcd.setCursor(6 + len, 2);
    lcd.setDisplay(true, true, true);
}

void ScreenEnterPassword::handleKey(char key) {
    const char correct[] = "12345";
    if (key >= '0' && key <= '9') {
        if (len < 5) {
            input[len++] = key;
            render();
            if (len == 5) {
                input[5] = '\0';
                if (strcmp(input, correct) == 0) {
                    requestState(SETTINGS_MENU);
                } else {
                    // неверный — вернуться в главное меню
                    requestState(MAIN_MENU);
                }
            }
        }
    } else if (key == 'W') {
        requestState(MAIN_MENU);
    }
}

// Удалён устаревший обёртывающий show-функция; используйте screenEnterPassword.render() напрямую
