#pragma once

struct Menu {
  const char** items;
  int8_t length;
  const char* title;
}; 

struct AnalogInputConfig {
  int8_t type;
  // Используем фиксированную точку (умноженную на 100), чтобы экономить RAM
  // вместо использования float
  int16_t minValueScaled;
  int16_t maxValueScaled;
  bool show;
};

struct MenuState {
  int8_t selectedIndex = 0;
  int8_t scroll = 0;
};

enum AppState { 
  MAIN_MENU, 
  ENTER_PASSWORD, 
  SETTINGS_MENU, 
  UART_MENU, 
  ETH_MENU, 
  DATE_TIME_MENU, 
  ANALOG_INPUTS_MENU,
  PARAMETERS_SCREEN,
  ANALOG_SETTINGS_MENU
};