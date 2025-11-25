// FutabaCyrillicLCD.h
// Библиотека для Futaba M204SD02A (8-бит, Motorola-режим) с UTF-8 кириллицей.

#ifndef FUTABA_CYRILLIC_LCD_H
#define FUTABA_CYRILLIC_LCD_H

#include <Arduino.h>

class FutabaCyrillicLCD {
public:
  /**
   * @param rs       — RS-пин (LOW=команда, HIGH=данные)
   * @param rw       — RW-пин (LOW=запись, HIGH=чтение busy-флага)
   * @param e        — Enable-пин
   * @param dataPins — массив из 8 пинов D0…D7
   */
  FutabaCyrillicLCD(uint8_t rs, uint8_t rw, uint8_t e, const uint8_t dataPins[8]);

  /** Настройка пинов, инициализация дисплея */
  void begin();

  /** Очистка экрана (CMD=0x01), возвращаемся домой */
  void clear();

  /** Возврат курсора домой (CMD=0x02) без очистки */
  void cursorHome();

  // Остальные команды из раздела 4-4:
  void setEntryMode(bool inc, bool shiftDisplay);
  void setDisplay(bool on, bool cursorOn, bool blinkOn);
  void shift(bool displayShift, bool toRight);
  void functionSet(bool data8bit, bool twoLines, bool font5x10);
  void setCGRAMAddr(uint8_t addr);
  void setDDRAMAddr(uint8_t addr);
  void setCursor(uint8_t col, uint8_t row);

  /** Вывод ASCII-строки напрямую */
  void print(const String &text);

  /** Вывод UTF-8 строки: ASCII без изменений, кириллица через таблицу */
  void printRus(const char *utf8);

  /** Запись одного байта в DDRAM/CGRAM */
  void writeChar(uint8_t chr);

  /** Создание пользовательского символа в CGRAM */
  void createChar(uint8_t location, uint8_t charmap[8]);

private:
  uint8_t  _rs, _rw, _e;
  uint8_t  _dataPins[8];

  /** Отправляет команду/данные и ждёт готовности BF=0 */
  void send(uint8_t value, bool isData);

  /** Устанавливает линии D0…D7 */
  void writeByte(uint8_t value);

  /** «Щёлкает» Enable: фронт ↑ и спадает ↓ */
  void pulseEnable();

  /** Ждём, пока Busy Flag (D7) не станет 0 */
  void waitBusy();

  /** Обратное чтение байта (для busy-флага) */
  uint8_t readByte();

  /** Декодирование одного UTF-8 символа в Unicode codepoint */
  uint16_t decodeUTF8(const char *&s);

  /** Поиск в таблице Unicode → код HD44780 */
  uint8_t mapRus(uint16_t codepoint);
};

#endif // FUTABA_CYRILLIC_LCD_H
