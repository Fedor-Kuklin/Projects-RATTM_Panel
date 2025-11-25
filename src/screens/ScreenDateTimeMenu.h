#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"

class ScreenDateTimeMenu : public Screen {
public:
	void enter() override;
	void exit() override;
	void update() override;
	void render() override;
	void save() override;
	void load() override;
    void handleKey(char key) override;
};

extern ScreenDateTimeMenu screenDateTimeMenu;
// Удалён устаревший обёртывающий show-функция; используйте screenDateTimeMenu.render()

#include <avr/pgmspace.h>
extern const char *const weekDaysRus[7] PROGMEM;
// Заголовок в PROGMEM объявлён в main.cpp
extern const char dateTimeTitle[] PROGMEM;