#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"

class ScreenAnalogInputsMenu : public Screen {
public:
	void enter() override;
	void exit() override;
	void update() override;
	void render() override;
	void save() override;
	void load() override;
	void handleKey(char key) override;
};

extern ScreenAnalogInputsMenu screenAnalogInputsMenu;
// Удалён устаревший обёртывающий show-функция; используйте screenAnalogInputsMenu.render()