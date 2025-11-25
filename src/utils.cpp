#include "utils.h"
#include <ArduinoModbus.h>
#include "screens/ScreenUARTMenu.h"
#include "screens/ScreenEthMenu.h"
#include "screens/ScreenAnalogInputsMenu.h"
#include "screens/ScreenDateTimeMenu.h"
#include "GlobalState.h"
#include <avr/pgmspace.h>
#include <string.h>

// Global scratch buffer used by screens for temporary formatting to avoid
// allocating large local buffers on the stack. Size chosen to cover LCD
// line (20 chars) + small formatting overhead.
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
  const int visibleRows = 3; // LCD rows used for items (header excluded)
  if (!menu) return;
  if (menu->length <= visibleRows) {
    state.scroll = 0;
    if (state.selectedIndex >= menu->length) state.selectedIndex = menu->length - 1;
    return;
  }
  // clamp selected
  if (state.selectedIndex < 0) state.selectedIndex = 0;
  if (state.selectedIndex >= menu->length) state.selectedIndex = menu->length - 1;

  int maxScroll = menu->length - visibleRows;
  if (state.selectedIndex < state.scroll) state.scroll = state.selectedIndex;
  if (state.selectedIndex >= state.scroll + visibleRows) state.scroll = state.selectedIndex - visibleRows + 1;
  if (state.scroll < 0) state.scroll = 0;
  if (state.scroll > maxScroll) state.scroll = maxScroll;
}

// --- PROGMEM helpers ---
void readProgmemString(PGM_P src, char *dst, size_t dstSize) {
  if (!dst || dstSize == 0) return;
  if (!src) { dst[0] = '\0'; return; }
  // Use strncpy_P which copies from program space
  strncpy_P(dst, (PGM_P)src, dstSize - 1);
  dst[dstSize - 1] = '\0';
}

void readProgmemTableString(const char *const table[] PROGMEM, uint8_t idx, char *dst, size_t dstSize) {
  if (!dst || dstSize == 0) return;
  // read pointer to string from PROGMEM table
  PGM_P p = (PGM_P)pgm_read_word(&(table[idx]));
  if (!p) { dst[0] = '\0'; return; }
  strncpy_P(dst, p, dstSize - 1);
  dst[dstSize - 1] = '\0';
}

// Request a state change from screens. This sets gState.nextState; the main
// loop will perform exit()/enter()/load()/render() when it sees the request.
// Behavior:
// - If requested state equals current state, the request is ignored.
// - If the same request is already pending, it is ignored.
// - Sets gState.deferRender to prevent synchronous render() calls in the
//   same handler from drawing the old screen after a transition request.
void requestState(AppState st) {
  // Ignore request to switch to the same state
  if ((uint8_t)st == gState.currentState) {
#if STATE_LOGGING
    if (Serial) Serial.println("requestState: ignored same state");
#endif
    return;
  }
  // Ignore duplicate pending request
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