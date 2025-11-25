#include "ScreenDateTimeMenu.h"
#include "../utils.h"

extern FutabaCyrillicLCD lcd;
extern Menu dateTimeMenu;
#include <avr/pgmspace.h>

static const char wd_0[] PROGMEM = "Понедельник";
static const char wd_1[] PROGMEM = "Вторник";
static const char wd_2[] PROGMEM = "Среда";
static const char wd_3[] PROGMEM = "Четверг";
static const char wd_4[] PROGMEM = "Пятница";
static const char wd_5[] PROGMEM = "Суббота";
static const char wd_6[] PROGMEM = "Воскресенье";
const char *const weekDaysRus[7] PROGMEM = {wd_0, wd_1, wd_2, wd_3, wd_4, wd_5, wd_6};

ScreenDateTimeMenu screenDateTimeMenu;

void ScreenDateTimeMenu::enter() {}
void ScreenDateTimeMenu::exit() {}
void ScreenDateTimeMenu::update() {}

void ScreenDateTimeMenu::render() {
    lcd.clear();
    
    // Заголовок (from PROGMEM)
    char tmpTitle[24];
    readProgmemString(dateTimeTitle, tmpTitle, sizeof(tmpTitle));
    int titleLen = utf8_strlen(tmpTitle);
    int titlePad = (20 - titleLen) / 2;
    lcd.setCursor(titlePad > 0 ? titlePad : 0, 0);
    lcd.printRus(tmpTitle);
    
    // Дата
    lcd.setCursor(0, 1);
    char *buf = g_scratch;
    snprintf(buf, sizeof(g_scratch), "Дата: %02u.%02u.%02u", gState.dt_day, gState.dt_month, gState.dt_year);
    lcd.printRus(buf);
    
    // День недели
    lcd.setCursor(0, 2);
    if (gState.dt_weekday >= 1 && gState.dt_weekday <= 7) {
        char tmp[24];
        readProgmemTableString(weekDaysRus, gState.dt_weekday - 1, tmp, sizeof(tmp));
        lcd.printRus(tmp);
    }
    
    // Время
    lcd.setCursor(0, 3);
    snprintf(buf, sizeof(g_scratch), "Время: %02u:%02u:%02u", gState.dt_hour, gState.dt_min, gState.dt_sec);
    lcd.printRus(buf);
}

void ScreenDateTimeMenu::save() {}
void ScreenDateTimeMenu::load() {}

// legacy wrapper removed; use screenDateTimeMenu.render() directly
void ScreenDateTimeMenu::handleKey(char key) {
    switch (key) {
        case 'E':
            // Обновить дату/время из регистров
            loadDateTimeFromRegisters();
            // если запросили смену состояния внутри load — не рендерим
            if (gState.nextState < 0) this->render();
            break;
        case 'W':
            // Вернуться в главное меню
            requestState(MAIN_MENU);
            return;
        default:
            break;
    }
}