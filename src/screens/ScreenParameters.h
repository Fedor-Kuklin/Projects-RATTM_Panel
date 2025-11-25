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

	// legacy handlers
	void handleKey(char key);
};

extern ScreenParameters screenParameters;
// legacy wrappers removed; use screenParameters.render() and
// screenParameters.handleKey(key) directly