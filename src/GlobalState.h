#pragma once
#include <stdint.h>
#include "MenuStructs.h"

struct GlobalState {
    // Состояния меню
    MenuState mainMenuState;
    MenuState settingsMenuState;
    MenuState analogMenuState;
    MenuState ethMenuState;
    MenuState uartMenuState;
    
    // Конфигурации
    uint8_t eth_ip[4] = {192, 168, 1, 1};
    uint8_t eth_mask[4] = {255, 255, 255, 0};
    uint8_t eth_gw[4] = {192, 168, 1, 1};
    uint8_t eth_dns[4] = {8, 8, 8, 8};
    
    uint32_t uart_baud = 9600;
    uint8_t uart_dataBits = 8;
    uint8_t uart_parity = 0;
    uint8_t uart_stopBits = 1;
    
    // Аналоговые входы
        // min/max масштабированы на 100
    AnalogInputConfig analogConfigs[5] = {
        {0, 0 * 100, 100 * 100, true},
        {0, 0 * 100, 100 * 100, true},
        {1, 0 * 100, 10 * 100, true},
        {1, 0 * 100, 10 * 100, true},
        {0, 0 * 100, 100 * 100, true}
    };
    
    uint16_t analogInputs[5] = {0, 0, 0, 0, 0};
        // Храним аналоговые значения в фиксированной точке (умноженные на 100)
        // чтобы экономить RAM по сравнению с float
    int16_t analogValuesScaled[5] = {0, 0, 0, 0, 0};
    
    // Дата и время
    uint8_t dt_day = 1, dt_month = 1, dt_year = 24;
    uint8_t dt_weekday = 1;
    uint8_t dt_hour = 0, dt_min = 0, dt_sec = 0;
    
    // Флаги редактирования (битовые флаги для экономии памяти)
        // FLAG_ETH_EDIT     - редактирование сетевых октетов
        // FLAG_UART_EDIT    - редактирование параметров UART
        // FLAG_ANALOG_EDIT  - редактирование настроек аналоговых входов
    uint8_t flags = 0;
    uint8_t ethEditOctet = 0;
    uint8_t uartEditIndex = 0;
    uint8_t analogSettingsEditField = 0;
    
    // Навигация (храним как uint8_t для экономии 1 байта на поле)
    uint8_t currentState = MAIN_MENU;
        // Запрос на смену состояния: экраны устанавливают nextState вместо
        // прямой установки currentState
    // -1 означает отсутствие запроса
    int8_t nextState = -1;
    uint8_t previousState = MAIN_MENU;
    uint8_t analogSettingsIndex = 0;
    uint8_t analogSettingsSelected = 0;

    // Если выставлен — предотвращать немедленную перерисовку текущего экрана
    // после запроса смены состояния (защита от гонок при синхронных вызовах).
    uint8_t deferRender = 0;

    // Состояние для экрана параметров
    int8_t paramsScroll = 0;
    
    // Пароль
    char inputPassword[6] = "";
    uint8_t passwordLength = 5;
};

extern GlobalState gState;

// Flags for GlobalState::flags
#define FLAG_ETH_EDIT       0x01
#define FLAG_UART_EDIT      0x02
#define FLAG_ANALOG_EDIT    0x04