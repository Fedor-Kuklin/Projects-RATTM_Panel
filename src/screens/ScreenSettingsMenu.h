#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"
#include <avr/pgmspace.h>

// Строки меню в PROGMEM объявлены в main.cpp
extern const char *const settingsItems[] PROGMEM;
extern const char settingsTitle[] PROGMEM;

class ScreenSettingsMenu : public Screen {
public:
	void enter() override;
	void exit() override;
	void update() override;
	void render() override;
	void save() override;
	void load() override;
	void handleKey(char key) override;
};

extern ScreenSettingsMenu screenSettingsMenu;
// Удалён устаревший обёртывающий show-функция; используйте screenSettingsMenu.render() через менеджер экранов