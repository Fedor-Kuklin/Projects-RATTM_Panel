#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"

#include <avr/pgmspace.h>
extern const char *const analogTypes[] PROGMEM;
extern const uint16_t ADC_4MA;
extern const uint16_t ADC_20MA;

class ScreenAnalogSettings : public Screen {
public:
	void enter() override;
	void exit() override;
	void update() override;
	void render() override;
	void save() override;
	void load() override;

	// Перегруженные методы для работы с индексом аналога
	void render(uint8_t analogIndex);
	void handleKey(char key, uint8_t analogIndex);
};

extern ScreenAnalogSettings screenAnalogSettings;
// Устаревшие обёртки удалены; используйте screenAnalogSettings.render(index)
// и screenAnalogSettings.handleKey(key, index) напрямую