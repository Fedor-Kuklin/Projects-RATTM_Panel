#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"
#include <avr/pgmspace.h>

// Заголовок в PROGMEM объявлён в main.cpp
extern const char ethMenuTitle[] PROGMEM;

class ScreenEthMenu : public Screen {
public:
	void enter() override;
	void exit() override;
	void update() override;
	void render() override;
	void save() override;
	void load() override;
	void handleKey(char key) override;
};

extern ScreenEthMenu screenEthMenu;
// Удалён устаревший обёртывающий show-функция; используйте screenEthMenu.render()