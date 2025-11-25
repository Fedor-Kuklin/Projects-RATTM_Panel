#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"

#include <avr/pgmspace.h>
extern const char *const mainMenuItems[] PROGMEM;
extern const char mainMenuTitle[] PROGMEM;

class ScreenMainMenu : public Screen {
public:
	void enter() override;
	void exit() override;
	void update() override;
	void render() override;
	void save() override;
	void load() override;
	void handleKey(char key) override;
};

extern ScreenMainMenu screenMainMenu;