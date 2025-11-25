
// #include <Arduino.h>
// #include "Futaba/FutabaCyrillicLCD.h"

// // Массив пинов D0…D7
// static const uint8_t dataPins[8] = {44, 43, 42, 41, 40, 39, 38, 37};

// static const uint8_t lcd_rs = 34;
// static const uint8_t lcd_rw = 33;
// static const uint8_t lcd_e = 2;

// // Объект дисплея: RS=34, RW=33, E=2
// FutabaCyrillicLCD lcd(lcd_rs, lcd_rw, lcd_e, dataPins);

// // Каркас батареи: слот 0 рендерит только контур
// uint8_t baseFrame[8] = {
//   0b01110,  // .###.
//   0b10001,  // #...#
//   0b10001,  // #...#
//   0b10001,  // #...#
//   0b10001,  // #...#
//   0b10001,  // #...#
//   0b10001,  // #...#
//   0b11111   // #####
// };

// /**
//  * Заполняет массив frame шаблоном батареи с заполнением снизу вверх.
//  * @param frame     — массив из 8 байт, куда пишем итоговый шаблон
//  * @param fillRows  — число заполненных внутренних строк (0–6)
//  */
// void buildBatteryVertical(uint8_t frame[8], uint8_t fillRows) {
//   // Копируем контур во все строки
//   memcpy(frame, baseFrame, 8);

//   // Внутренние горизонтальные линии лежат в строках 1…6 (6 строк)
//   // Если строка номер row должна быть заполнена снизу вверх —
//   // заполняем центральные биты (1,2,3) => маска 0b01110
//   for (uint8_t row = 1; row <= 6; row++) {
//     // считаем, сколько строк от низа (6) вверх мы заполняем
//     if (row > 6 - fillRows) {
//       frame[row] |= 0b01110;
//     }
//   }
// }

// void setup() {
//   lcd.begin();
//   lcd.setCursor(0, 0);
//   // Рисуем пустую батарею первым вызовом createChar
//   lcd.createChar(0, baseFrame);
//   lcd.writeChar(0);
// }

// void loop() {
//   // Демонстрация: заполняем от 0 до 6 строк снизу вверх
//   for (uint8_t lvl = 0; lvl <= 6; lvl++) {
//     uint8_t battChar[8];
//     buildBatteryVertical(battChar, lvl);

//     // Перезаписываем слот 0 — все существующие символы обновятся
//     lcd.createChar(0, battChar);
//     delay(800);
//   }

//   // Затем очищаем и повторяем
//   delay(1200);
// }