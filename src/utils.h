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
// Вспомогательная утилита, используемая экранами/main для прокрутки меню
void updateMenuScroll(struct MenuState &state, const struct Menu *menu);

// Вспомогательные функции для работы с PROGMEM
// Копирует NUL-терминированную строку из PROGMEM (источник PGM_P) в dst.
// dstSize включает место для завершающего NUL.
void readProgmemString(PGM_P src, char *dst, size_t dstSize);

// Копировать строку из таблицы строк в PROGMEM: таблица объявляется как
// `const char *const table[] PROGMEM` у вызывающего. idx — индекс записи.
void readProgmemTableString(const char *const table[] PROGMEM, uint8_t idx, char *dst, size_t dstSize);

// Предварительное объявление AppState
#include "MenuStructs.h"

// Запросить переход состояния из экрана: экраны должны вызывать это вместо
// прямой записи в gState.currentState. Функция устанавливает gState.nextState
// и позволяет main циклу безопасно выполнить exit()/enter().
void requestState(AppState st);

// Глобальный временный буфер для временного форматирования, чтобы снизить
// использование стека. Размер выбран для типичных строк ЖКИ (~20 знаков)
// плюс форматирование.
extern char g_scratch[32];

// Включить лёгкое логирование переходов состояний для отладки (0 = выкл)
#ifndef STATE_LOGGING
#define STATE_LOGGING 0
#endif