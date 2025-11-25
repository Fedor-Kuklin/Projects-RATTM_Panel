#include "ScreenUARTMenu.h"
#include "../utils.h"

extern FutabaCyrillicLCD lcd;
extern Menu uartMenu;

// UART value lists
const uint32_t baudRates[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600};
const uint8_t baudRatesCount = 7;
const uint8_t dataBitsList[] = {8, 7};
const uint8_t dataBitsCount = 2;

const uint8_t stopBitsList[] = {1, 2};
const uint8_t stopBitsCount = 2;

// Parity list stored in PROGMEM
static const char parity_0[] PROGMEM = "N";
static const char parity_1[] PROGMEM = "O";
static const char parity_2[] PROGMEM = "E";
const char *const parityList[] PROGMEM = {parity_0, parity_1, parity_2};
const uint8_t parityCount = 3;

ScreenUARTMenu screenUARTMenu;

void ScreenUARTMenu::enter() {}
void ScreenUARTMenu::exit() {}
void ScreenUARTMenu::update() {}

void ScreenUARTMenu::render() {
    lcd.clear();

    // заголовок
    char tmpTitle[24];
    readProgmemString(uartMenuTitle, tmpTitle, sizeof(tmpTitle));
    int titleLen = utf8_strlen(tmpTitle);
    int titlePad = (20 - titleLen) / 2;
    lcd.setCursor(titlePad > 0 ? titlePad : 0, 0);
    lcd.printRus(tmpTitle);

    // ункты меню с значениями (строки 1..3 на дисплее)
    for (int i = 0; i < 3; i++) {
        int itemIdx = i + gState.uartMenuState.scroll;
        lcd.setCursor(0, i + 1);
        if (itemIdx >= uartMenu.length) continue;

    // стрелка выбора
    lcd.writeChar((itemIdx == gState.uartMenuState.selectedIndex) ? 0x10 : ' ');

    // название пункта из PROGMEM
    char tmp[24];
    readProgmemTableString(uartMenuItems, itemIdx, tmp, sizeof(tmp));
    lcd.printRus(tmp);

        // значение пункта (правая часть строки)
        char buf[16];
        switch (itemIdx) {
            case 0: // baud
                if ((gState.flags & FLAG_UART_EDIT) && gState.uartMenuState.selectedIndex == 0) {
                    snprintf(buf, sizeof(buf), "%lu <", gState.uart_baud);
                } else {
                    snprintf(buf, sizeof(buf), "%lu", gState.uart_baud);
                }
                lcd.setCursor(12, i + 1);
                lcd.print(buf);
                break;
            case 1: // data bits
                if ((gState.flags & FLAG_UART_EDIT) && gState.uartMenuState.selectedIndex == 1) {
                    snprintf(buf, sizeof(buf), "%u <", dataBitsList[gState.uartEditIndex]);
                } else {
                    snprintf(buf, sizeof(buf), "%u", gState.uart_dataBits);
                }
                lcd.setCursor(12, i + 1);
                lcd.print(buf);
                break;
            case 2: // parity
                if ((gState.flags & FLAG_UART_EDIT) && gState.uartMenuState.selectedIndex == 2) {
                    char ptmp[8];
                    readProgmemTableString(parityList, gState.uartEditIndex, ptmp, sizeof(ptmp));
                    lcd.setCursor(12, i + 1);
                    lcd.printRus(ptmp);
                    lcd.print(" <");
                } else {
                    char ptmp[8];
                    readProgmemTableString(parityList, gState.uart_parity, ptmp, sizeof(ptmp));
                    lcd.setCursor(12, i + 1);
                    lcd.printRus(ptmp);
                }
                break;
            case 3: // stop bits
                if ((gState.flags & FLAG_UART_EDIT) && gState.uartMenuState.selectedIndex == 3) {
                    snprintf(buf, sizeof(buf), "%u <", stopBitsList[gState.uartEditIndex]);
                } else {
                    snprintf(buf, sizeof(buf), "%u", gState.uart_stopBits);
                }
                lcd.setCursor(12, i + 1);
                lcd.print(buf);
                break;
            case 4: // apply
                // nothing to the right
                break;
        }
    }
}

void ScreenUARTMenu::save() {}
void ScreenUARTMenu::load() {}

// legacy wrapper removed; use screenUARTMenu.render() directly

void ScreenUARTMenu::handleKey(char key) {
    if (!(gState.flags & FLAG_UART_EDIT)) {
        switch (key) {
            case 'U':
                gState.uartMenuState.selectedIndex = (gState.uartMenuState.selectedIndex > 0) ? 
                    gState.uartMenuState.selectedIndex - 1 : uartMenu.length - 1;
                break;
            case 'D':
                gState.uartMenuState.selectedIndex = (gState.uartMenuState.selectedIndex < uartMenu.length - 1) ? 
                    gState.uartMenuState.selectedIndex + 1 : 0;
                break;
            case 'E':
                if (gState.uartMenuState.selectedIndex < 4) {
                    gState.flags |= FLAG_UART_EDIT;
                    switch (gState.uartMenuState.selectedIndex) {
                        case 0:
                            for (uint8_t i = 0; i < baudRatesCount; i++) if (baudRates[i] == gState.uart_baud) gState.uartEditIndex = i;
                            break;
                        case 1:
                            for (uint8_t i = 0; i < dataBitsCount; i++) if (dataBitsList[i] == gState.uart_dataBits) gState.uartEditIndex = i;
                            break;
                        case 2:
                            gState.uartEditIndex = gState.uart_parity;
                            break;
                        case 3:
                            for (uint8_t i = 0; i < stopBitsCount; i++) if (stopBitsList[i] == gState.uart_stopBits) gState.uartEditIndex = i;
                            break;
                    }
                } else if (gState.uartMenuState.selectedIndex == 4) {
                    saveUARTToRegisters();
                    requestState(SETTINGS_MENU);
                    return;
                }
                break;
            case 'W':
                requestState(SETTINGS_MENU);
                return;
        }
        updateMenuScroll(gState.uartMenuState, &uartMenu);
    } else {
        switch (key) {
            case 'U':
                switch (gState.uartMenuState.selectedIndex) {
                    case 0: gState.uartEditIndex = (gState.uartEditIndex > 0) ? gState.uartEditIndex - 1 : baudRatesCount - 1; break;
                    case 1: gState.uartEditIndex = (gState.uartEditIndex > 0) ? gState.uartEditIndex - 1 : dataBitsCount - 1; break;
                    case 2: gState.uartEditIndex = (gState.uartEditIndex > 0) ? gState.uartEditIndex - 1 : parityCount - 1; break;
                    case 3: gState.uartEditIndex = (gState.uartEditIndex > 0) ? gState.uartEditIndex - 1 : stopBitsCount - 1; break;
                }
                break;
            case 'D':
                switch (gState.uartMenuState.selectedIndex) {
                    case 0: gState.uartEditIndex = (gState.uartEditIndex < baudRatesCount - 1) ? gState.uartEditIndex + 1 : 0; break;
                    case 1: gState.uartEditIndex = (gState.uartEditIndex < dataBitsCount - 1) ? gState.uartEditIndex + 1 : 0; break;
                    case 2: gState.uartEditIndex = (gState.uartEditIndex < parityCount - 1) ? gState.uartEditIndex + 1 : 0; break;
                    case 3: gState.uartEditIndex = (gState.uartEditIndex < stopBitsCount - 1) ? gState.uartEditIndex + 1 : 0; break;
                }
                break;
            case 'E':
                switch (gState.uartMenuState.selectedIndex) {
                    case 0: gState.uart_baud = baudRates[gState.uartEditIndex]; break;
                    case 1: gState.uart_dataBits = dataBitsList[gState.uartEditIndex]; break;
                    case 2: gState.uart_parity = gState.uartEditIndex; break;
                    case 3: gState.uart_stopBits = stopBitsList[gState.uartEditIndex]; break;
                }
                gState.flags &= ~FLAG_UART_EDIT;
                break;
            case 'W':
                gState.flags &= ~FLAG_UART_EDIT;
                break;
        }
    }
    this->render();
}
