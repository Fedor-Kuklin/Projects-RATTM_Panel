#include "ScreenEthMenu.h"
#include "../utils.h"

extern FutabaCyrillicLCD lcd;
extern Menu ethMenu;

ScreenEthMenu screenEthMenu;

void ScreenEthMenu::enter() {}
void ScreenEthMenu::exit() {}
void ScreenEthMenu::update() {}

void ScreenEthMenu::render() {
    lcd.clear();
    
    // Заголовок (из PROGMEM)
    char tmpTitle[24];
    readProgmemString(ethMenuTitle, tmpTitle, sizeof(tmpTitle));
    int titleLen = utf8_strlen(tmpTitle);
    int titlePad = (20 - titleLen) / 2;
    lcd.setCursor(titlePad > 0 ? titlePad : 0, 0);
    lcd.printRus(tmpTitle);
    
    // Пункты меню с значениями
    for (int i = 0; i < 3; i++) {
        int itemIdx = i + gState.ethMenuState.scroll;
        lcd.setCursor(0, i + 1);
        
        if (itemIdx < ethMenu.length) {
            lcd.writeChar((itemIdx == gState.ethMenuState.selectedIndex) ? 0x10 : ' ');
            
            char *buf = g_scratch;
            const uint8_t* arr = nullptr;
            
            switch (itemIdx) {
                case 0: 
                    arr = gState.eth_ip; 
                    lcd.printRus("IP: ");
                    break;
                case 1: 
                    arr = gState.eth_mask; 
                    lcd.printRus("Mask: ");
                    break;
                case 2: 
                    arr = gState.eth_gw; 
                    lcd.printRus("GW: ");
                    break;
                case 3: 
                    arr = gState.eth_dns; 
                    lcd.printRus("DNS: ");
                    break;
                case 4:
                    lcd.printRus("Применить");
                    break;
            }
            
            if (itemIdx < 4 && arr) {
                for (int j = 0; j < 4; j++) {
                    if ((gState.flags & FLAG_ETH_EDIT) && gState.ethMenuState.selectedIndex == itemIdx && gState.ethEditOctet == j) {
                        lcd.print("[");
                    }
                    snprintf(buf, sizeof(g_scratch), "%u", arr[j]);
                    lcd.print(buf);
                    if ((gState.flags & FLAG_ETH_EDIT) && gState.ethMenuState.selectedIndex == itemIdx && gState.ethEditOctet == j) {
                        lcd.print("]");
                    }
                    if (j < 3) lcd.print(".");
                }
            }
        }
    }
}

void ScreenEthMenu::save() {}
void ScreenEthMenu::load() {}

// Удалён устаревший обёртывающий show-функция; используйте screenEthMenu.render() напрямую

void ScreenEthMenu::handleKey(char key) {
    if (!(gState.flags & FLAG_ETH_EDIT)) {
        switch (key) {
            case 'U':
                gState.ethMenuState.selectedIndex = (gState.ethMenuState.selectedIndex > 0) ? 
                        gState.ethMenuState.selectedIndex - 1 : ethMenu.length - 1;
                break;
            case 'D':
                gState.ethMenuState.selectedIndex = (gState.ethMenuState.selectedIndex < ethMenu.length - 1) ? 
                        gState.ethMenuState.selectedIndex + 1 : 0;
                break;
            case 'E':
                        if (gState.ethMenuState.selectedIndex < 4) {
                            gState.flags |= FLAG_ETH_EDIT;
                            gState.ethEditOctet = 0;
                        } else if (gState.ethMenuState.selectedIndex == 4) {
                            saveEthToRegisters();
                            requestState(SETTINGS_MENU);
                            return;
                        }
                break;
                    case 'W':
                        requestState(SETTINGS_MENU);
                        return;
        }
        updateMenuScroll(gState.ethMenuState, &ethMenu);
    } else {
        uint8_t* editArray = nullptr;
        switch (gState.ethMenuState.selectedIndex) {
            case 0: editArray = gState.eth_ip; break;
            case 1: editArray = gState.eth_mask; break;
            case 2: editArray = gState.eth_gw; break;
            case 3: editArray = gState.eth_dns; break;
        }
        if (editArray) {
            switch (key) {
                case 'U':
                    editArray[gState.ethEditOctet] = (editArray[gState.ethEditOctet] < 255) ? editArray[gState.ethEditOctet] + 1 : 0;
                    break;
                case 'D':
                    editArray[gState.ethEditOctet] = (editArray[gState.ethEditOctet] > 0) ? editArray[gState.ethEditOctet] - 1 : 255;
                    break;
                case 'E':
                    if (gState.ethEditOctet < 3) {
                        gState.ethEditOctet++;
                    } else {
                        gState.flags &= ~FLAG_ETH_EDIT;
                        gState.ethEditOctet = 0;
                    }
                    break;
                case 'W':
                    gState.flags &= ~FLAG_ETH_EDIT;
                    gState.ethEditOctet = 0;
                    break;
            }
        }
    }
    // Повторная перерисовка текущего экрана
    this->render();
}