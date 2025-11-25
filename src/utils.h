#pragma once
#include <stdint.h>
#include <avr/pgmspace.h>
#include <stddef.h>

uint8_t utf8_strlen(const char* str);

// --- Объявления функций для работы с регистрами ---
void saveUARTToRegisters();
void loadUARTFromRegisters();
void loadAnalogInputsFromRegisters();
void loadDateTimeFromRegisters();
void saveEthToRegisters();
void loadEthFromRegisters();
// Utility used by screens/main for menu scrolling
void updateMenuScroll(struct MenuState &state, const struct Menu *menu);

// PROGMEM helpers
// Copy a NUL-terminated string from PROGMEM (source PGM_P) into dst
// dstSize includes space for terminating NUL.
void readProgmemString(PGM_P src, char *dst, size_t dstSize);

// Copy a string from a PROGMEM table of strings: table is declared as
// `const char *const table[] PROGMEM` in the callers. idx selects the entry.
void readProgmemTableString(const char *const table[] PROGMEM, uint8_t idx, char *dst, size_t dstSize);

// Forward AppState
#include "MenuStructs.h"

// Request a state transition from a screen: screens should call this instead
// of writing to gState.currentState directly. This sets gState.nextState and
// lets the main loop perform exit()/enter() safely.
void requestState(AppState st);

// Global scratch buffer for temporary formatting to reduce stack usage.
// Size chosen to cover typical LCD lines (20 chars) plus formatting.
extern char g_scratch[32];

// Enable lightweight state transition logging for debugging (0 = off)
#ifndef STATE_LOGGING
#define STATE_LOGGING 0
#endif