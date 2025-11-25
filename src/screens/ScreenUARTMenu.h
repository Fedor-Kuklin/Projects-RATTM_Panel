#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"

// Списки значений для настроек UART
extern const uint32_t baudRates[];
extern const uint8_t baudRatesCount;
extern const uint8_t dataBitsList[];
extern const uint8_t dataBitsCount;
#include <avr/pgmspace.h>
extern const char *const parityList[] PROGMEM;
extern const uint8_t parityCount;
extern const uint8_t stopBitsList[];
extern const uint8_t stopBitsCount;
// Строки меню в PROGMEM объявлены в main.cpp
extern const char *const uartMenuItems[] PROGMEM;
extern const char uartMenuTitle[] PROGMEM;

class ScreenUARTMenu : public Screen {
public:
	void enter() override;
	void exit() override;
	void update() override;
	void render() override;
	void save() override;
	void load() override;
	void handleKey(char key) override;
};

extern ScreenUARTMenu screenUARTMenu;
// Удалён устаревший обёртывающий show-функция; используйте screenUARTMenu.render()