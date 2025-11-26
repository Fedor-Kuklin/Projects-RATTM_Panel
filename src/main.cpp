#include <Arduino.h>
#include "RS485.h"
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "Keypad/ExternalKeyPad.h"
#include "Futaba/FutabaCyrillicLCD.h"
#include <GTimer.h>

// Пошаговая миграция: подключаем заголовки экранов и объявления утилит,
// чтобы main мог делегировать ввод и отрисовку подклассам Screen при их наличии.
#include "utils.h"
#include "GlobalState.h"
#include "MenuStructs.h"

#include "screens/ScreenMainMenu.h"
#include "screens/ScreenSettingsMenu.h"
#include "screens/ScreenUARTMenu.h"
#include "screens/ScreenEthMenu.h"
#include "screens/ScreenDateTimeMenu.h"
#include "screens/ScreenAnalogInputsMenu.h"
#include "screens/ScreenAnalogSettings.h"
#include "screens/ScreenParameters.h"
#include "screens/ScreenEnterPassword.h"

#include <avr/pgmspace.h>

static GTimer<millis> tmr3;

const uint8_t dataPins[8] = {44, 43, 42, 41, 40, 39, 38, 37};
FutabaCyrillicLCD lcd(34, 33, 2, dataPins);
const int TX_PIN = 21;
const int RX_PIN = 20;
const int DE_PIN = 25;
const int RE_PIN = 25;
ModbusRTUServerClass RTU;
RS485Class rs485 = RS485Class(Serial1, TX_PIN, DE_PIN, RE_PIN);

unsigned long lastActivity = 0;
bool lcdIsOn = true;
const unsigned long LCD_TIMEOUT = 10000; // 10 секунд в мс

// Метаданные меню, используемые экранами (заголовки/пункты). Строки перемещены
// в PROGMEM для экономии оперативной памяти.
const char m0[] PROGMEM = "Экран параметров";
const char m1[] PROGMEM = "Настройки";
const char m2[] PROGMEM = "Диагностика";
const char m3[] PROGMEM = "Аналоговые входа";
const char m4[] PROGMEM = "Дата и время";
const char *const mainMenuItems[] PROGMEM = {m0, m1, m2, m3, m4};
const char mainMenuTitle[] PROGMEM = "Главное меню";

const char s0[] PROGMEM = "Настройки UART";
const char s1[] PROGMEM = "Настройки Eth";
const char s2[] PROGMEM = "Настройки времени";
const char *const settingsItems[] PROGMEM = {s0, s1, s2};
const char settingsTitle[] PROGMEM = "Настройки";

const char u0[] PROGMEM = "Скорость";
const char u1[] PROGMEM = "Бит данных";
const char u2[] PROGMEM = "Паритет";
const char u3[] PROGMEM = "Стоп бит";
const char u4[] PROGMEM = "Применить";
const char *const uartMenuItems[] PROGMEM = {u0, u1, u2, u3, u4};
const char uartMenuTitle[] PROGMEM = "UART настройки";

const char e0[] PROGMEM = "IP";
const char e1[] PROGMEM = "Mask";
const char e2[] PROGMEM = "Gateway";
const char e3[] PROGMEM = "DNS";
const char e4[] PROGMEM = "Применить";
const char *const ethMenuItems[] PROGMEM = {e0, e1, e2, e3, e4};
const char ethMenuTitle[] PROGMEM = "Eth настройки";

const char a0[] PROGMEM = "Аналоговый 1";
const char a1[] PROGMEM = "Аналоговый 2";
const char a2[] PROGMEM = "Аналоговый 3";
const char a3[] PROGMEM = "Аналоговый 4";
const char a4[] PROGMEM = "Аналоговый 5";
const char *const analogInputsMenuItems[] PROGMEM = {a0, a1, a2, a3, a4};
const char analogInputsTitle[] PROGMEM = "Аналоговые входа";

const char dt0[] PROGMEM = "Дата и время";
const char *const dateTimeMenuItems[] PROGMEM = {dt0};
const char dateTimeTitle[] PROGMEM = "Дата и время";

Menu ethMenu = {nullptr, sizeof(ethMenuItems)/sizeof(ethMenuItems[0]), nullptr};
Menu uartMenu = {nullptr, sizeof(uartMenuItems)/sizeof(uartMenuItems[0]), nullptr};
Menu mainMenu = {nullptr, sizeof(mainMenuItems)/sizeof(mainMenuItems[0]), nullptr};
Menu settingsMenu = {nullptr, sizeof(settingsItems)/sizeof(settingsItems[0]), nullptr};
Menu dateTimeMenu = {nullptr, 1, nullptr};
Menu analogInputsMenu = {nullptr, sizeof(analogInputsMenuItems)/sizeof(analogInputsMenuItems[0]), nullptr};

// инициализация
void setup() {
  Serial.begin(57600);
  while (!Serial);
  delay(2000);
  if (!RTU.begin(rs485, 1, 9600)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }
  RTU.configureHoldingRegisters(0x00, 100);
  lcd.begin();
  // Стартуем с главного экрана через объект screenMainMenu
  screenMainMenu.enter();
  screenMainMenu.load();
  screenMainMenu.render();
}

// основной цикл
void loop() {
  RTU.poll();
  static GTimer<millis> tmr3_local(LCD_TIMEOUT, true);
  if (tmr3_local) {
    tmr3_local.stop();
    lcdIsOn = false;
    lcd.setDisplay(false, false, false);
  }

  static AppState prevState = (AppState)-1;
  static Screen* currentScreenPtr = nullptr;

  bool needRedraw = false;
  char key = customKeypad.getKey();
  if (gState.currentState != prevState) needRedraw = true;

  if (key) {
    tmr3_local.start();
    if (!lcdIsOn) {
      lcdIsOn = true;
      lcd.setDisplay(true, false, false);
    }
  }

  auto getScreenForState = [&](AppState st) -> Screen* {
    switch (st) {
      case MAIN_MENU: return &screenMainMenu;
      case ENTER_PASSWORD: return &screenEnterPassword;
      case SETTINGS_MENU: return &screenSettingsMenu;
      case UART_MENU: return &screenUARTMenu;
      case ETH_MENU: return &screenEthMenu;
      case DATE_TIME_MENU: return &screenDateTimeMenu;
      case ANALOG_INPUTS_MENU: return &screenAnalogInputsMenu;
      case PARAMETERS_SCREEN: return &screenParameters;
      case ANALOG_SETTINGS_MENU: return &screenAnalogSettings;
      default: return nullptr;
    }
  };
  // Получить объект экрана по значению состояния (AppState -> Screen*)
  // Используется для централизованного управления переходами и вызова методов экранов.

  // Если экран запросил смену состояния — выполним переход централизованно.
  // Шаги: вызвать exit() для старого экрана, обновить gState.currentState,
  // сбросить deferRender, получить объект нового экрана и выполнить enter()/load()/render().
  if (gState.nextState >= 0) {
    AppState requested = (AppState)gState.nextState;
    if (currentScreenPtr) currentScreenPtr->exit();
    gState.currentState = (uint8_t)requested;
    gState.nextState = -1;
  // Сбрасываем deferRender — сейчас будем отрисовывать новый экран в контролируемом порядке
    gState.deferRender = 0;
    currentScreenPtr = getScreenForState(requested);
    if (currentScreenPtr) {
      currentScreenPtr->enter();
      currentScreenPtr->load();
      currentScreenPtr->render();
    }
    prevState = requested;
    return;
  }

  AppState currentState = (AppState)gState.currentState;

  // Если текущее состояние отличается от предыдущего (например, внешний сброс состояния),
  // инициируем вход в новый экран: exit для старого, enter/load/render для нового.
  if (currentState != prevState) {
    if (currentScreenPtr) currentScreenPtr->exit();
    currentScreenPtr = getScreenForState(currentState);
    if (currentScreenPtr) {
      currentScreenPtr->enter();
      currentScreenPtr->load();
      currentScreenPtr->render();
    }
    prevState = currentState;
    return;
  }

  // Обычный рабочий цикл для активного экрана:
  // - обработка нажатий через handleKey
  // - периодическое обновление через update
  // - перерисовка, если флаг needRedraw выставлен
  if (currentScreenPtr) {
    if (key) currentScreenPtr->handleKey(key);
    currentScreenPtr->update();
    if (needRedraw) currentScreenPtr->render();
    prevState = currentState;
    return;
  }
  // Если для текущего состояния нет объекта экрана — ничего не делать.
  prevState = currentState;
}