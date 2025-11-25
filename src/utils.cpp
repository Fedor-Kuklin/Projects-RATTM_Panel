#include "utils.h"
#include <ArduinoModbus.h>
#include "screens/ScreenUARTMenu.h"
#include "screens/ScreenEthMenu.h"
#include "screens/ScreenAnalogInputsMenu.h"
#include "screens/ScreenDateTimeMenu.h"
#include "GlobalState.h"
#include <avr/pgmspace.h>
#include <string.h>

// Глобальный временный буфер, используемый экранами для форматирования,
// чтобы не выделять большие локальные буферы в стеке. Размер рассчитан на
// строку ЖКИ (20 символов) и небольшую служебную часть.
char g_scratch[32];

extern ModbusRTUServerClass RTU;

uint8_t utf8_strlen(const char* str) {
  uint8_t len = 0;
  while (*str) {
    if ((*str & 0xC0) != 0x80) len++;
    str++;
  }
  return len;
}

void loadUARTFromRegisters() {
    uint32_t reg_baud = RTU.holdingRegisterRead(40);
    gState.uart_baud = reg_baud ? reg_baud : 9600;
    
    uint8_t reg_dataBits = RTU.holdingRegisterRead(41);
    gState.uart_dataBits = reg_dataBits ? reg_dataBits : 8;
    
    uint8_t reg_parity = RTU.holdingRegisterRead(42);
    gState.uart_parity = reg_parity ? reg_parity : 0;
    
    uint8_t reg_stopBits = RTU.holdingRegisterRead(43);
    gState.uart_stopBits = reg_stopBits ? reg_stopBits : 1;
    
    // Устанавливаем начальные индексы для редактирования
    for (uint8_t i = 0; i < baudRatesCount; i++) {
        if (baudRates[i] == gState.uart_baud) {
            gState.uartEditIndex = i;
            break;
        }
    }
}

void saveUARTToRegisters() {
  RTU.holdingRegisterWrite(44, gState.uart_baud);
  RTU.holdingRegisterWrite(45, gState.uart_dataBits);
  RTU.holdingRegisterWrite(46, gState.uart_parity);
  RTU.holdingRegisterWrite(47, gState.uart_stopBits);
}

void loadEthFromRegisters() {
  for (int i = 0; i < 4; i++) {
    gState.eth_ip[i] = RTU.holdingRegisterRead(i);
    gState.eth_mask[i] = RTU.holdingRegisterRead(4 + i);
    gState.eth_gw[i] = RTU.holdingRegisterRead(8 + i);
    gState.eth_dns[i] = RTU.holdingRegisterRead(12 + i);
  }
}

void saveEthToRegisters() {
  for (int i = 0; i < 4; i++) {
    RTU.holdingRegisterWrite(20 + i, gState.eth_ip[i]);
    RTU.holdingRegisterWrite(24 + i, gState.eth_mask[i]);
    RTU.holdingRegisterWrite(28 + i, gState.eth_gw[i]);
    RTU.holdingRegisterWrite(32 + i, gState.eth_dns[i]);
  }
  RTU.holdingRegisterWrite(19, 1);
}

void loadAnalogInputsFromRegisters() {
  for (int i = 0; i < 5; i++) {
    gState.analogInputs[i] = RTU.holdingRegisterRead(50 + i);
  }
}

void loadDateTimeFromRegisters() {
  gState.dt_day = RTU.holdingRegisterRead(24);
  gState.dt_month = RTU.holdingRegisterRead(25);
  gState.dt_year = RTU.holdingRegisterRead(26);
  gState.dt_weekday = RTU.holdingRegisterRead(23);
  gState.dt_hour = RTU.holdingRegisterRead(22);
  gState.dt_min = RTU.holdingRegisterRead(21);
  gState.dt_sec = RTU.holdingRegisterRead(20);
}

// Обновление прокрутки меню — поддержка общего поведения для экранов
void updateMenuScroll(struct MenuState &state, const struct Menu *menu) {
  const int visibleRows = 3; // число видимых строк ЖКИ для пунктов (заголовок исключён)
  if (!menu) return;
  if (menu->length <= visibleRows) {
    state.scroll = 0;
    if (state.selectedIndex >= menu->length) state.selectedIndex = menu->length - 1;
    return;
  }
  // ограничить selected
  if (state.selectedIndex < 0) state.selectedIndex = 0;
  if (state.selectedIndex >= menu->length) state.selectedIndex = menu->length - 1;

  int maxScroll = menu->length - visibleRows;
  if (state.selectedIndex < state.scroll) state.scroll = state.selectedIndex;
  if (state.selectedIndex >= state.scroll + visibleRows) state.scroll = state.selectedIndex - visibleRows + 1;
  if (state.scroll < 0) state.scroll = 0;
  if (state.scroll > maxScroll) state.scroll = maxScroll;
}

// --- Вспомогательные функции для PROGMEM ---
void readProgmemString(PGM_P src, char *dst, size_t dstSize) {
  if (!dst || dstSize == 0) return;
  if (!src) { dst[0] = '\0'; return; }
  // Используем strncpy_P для копирования из области программной памяти (PROGMEM)
  strncpy_P(dst, (PGM_P)src, dstSize - 1);
  dst[dstSize - 1] = '\0';
}

void readProgmemTableString(const char *const table[] PROGMEM, uint8_t idx, char *dst, size_t dstSize) {
  if (!dst || dstSize == 0) return;
  // читаем указатель на строку из таблицы в PROGMEM
  PGM_P p = (PGM_P)pgm_read_word(&(table[idx]));
  if (!p) { dst[0] = '\0'; return; }
  strncpy_P(dst, p, dstSize - 1);
  dst[dstSize - 1] = '\0';
}

// Запрос смены состояния от экранов. Устанавливает gState.nextState; main
// цикл выполнит exit()/enter()/load()/render() при обнаружении запроса.
// Поведение:
// - Если запрошенное состояние совпадает с текущим — запрос игнорируется.
// - Если такой же запрос уже ожидает выполнения — он игнорируется.
// - Устанавливается gState.deferRender, чтобы предотвратить синхронные
//   вызовы render() в текущем обработчике, которые могли бы отрисовать старый
//   экран после запроса перехода.
void requestState(AppState st) {
  // Игнорировать запрос перехода в то же самое состояние
  if ((uint8_t)st == gState.currentState) {
#if STATE_LOGGING
    if (Serial) Serial.println("requestState: ignored same state");
#endif
    return;
  }
  // Игнорировать дублирующий ожидающийся запрос
  if (gState.nextState >= 0 && (int8_t)st == gState.nextState) {
#if STATE_LOGGING
    if (Serial) Serial.println("requestState: duplicate request ignored");
#endif
    return;
  }
  gState.nextState = (int8_t)st;
  gState.deferRender = 1; // prevent immediate render in current handler
#if STATE_LOGGING
  if (Serial) {
    Serial.print("requestState: requested -> ");
    Serial.println((int)st);
  }
#endif
}