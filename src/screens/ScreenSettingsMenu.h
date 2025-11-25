#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"
#include <avr/pgmspace.h>

// PROGMEM menu strings declared in main.cpp
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
// legacy wrapper removed; use screenSettingsMenu.render() via ScreenManager