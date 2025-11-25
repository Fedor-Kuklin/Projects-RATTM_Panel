#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"
#include <stdint.h>

class ScreenParameters : public Screen {
public:
	void enter() override;
	void exit() override;
	void update() override;
	void render() override;
	void save() override;
	void load() override;

	// Устаревшие обработчики
	void handleKey(char key);
};

extern ScreenParameters screenParameters;
// Устаревшие обёртки удалены; используйте screenParameters.render() и
// screenParameters.handleKey(key) напрямую