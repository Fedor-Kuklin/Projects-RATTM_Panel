// FutabaCyrillicLCD.cpp
// Реализация методов библиотеки FutabaCyrillicLCD

#include "FutabaCyrillicLCD.h"

// Запись соответствий Unicode → код дисплея HD44780
struct CharMapEntry {
  uint16_t unicode;  // Unicode codepoint
  uint8_t  lcdCode;  // Код символа в таблице HD44780
};

static const CharMapEntry rusCharMap[] = {
  {0x0410, 0x41},  // А → 0x80
  {0x0411, 0x80},  // Б → 0x81
  {0x0412, 0x42},  // В → 0x82
  {0x0413, 0x92},  // Г → 0x83
  {0x0414, 0x81},  // Д → 0x84
  {0x0415, 0x45},  // Е → 0x85
  {0x0416, 0x82},  // Ж → 0x86
  {0x0417, 0x83},  // З → 0x87
  {0x0418, 0x84},  // И → 0x88
  {0x0419, 0x85},  // Й → 0x89
  {0x041A, 0x4B},  // К → 0x8A
  {0x041B, 0x86},  // Л → 0x8B
  {0x041C, 0x4D},  // М → 0x8C
  {0x041D, 0x48},  // Н → 0x8D
  {0x041E, 0x4F},  // О → 0x8E
  {0x041F, 0x87},  // П → 0x8F
  {0x0420, 0x50},  // Р → 0x90
  {0x0421, 0x43},  // С → 0x91
  {0x0422, 0x54},  // Т → 0x92
  {0x0423, 0x88},  // У → 0x93
  {0x0424, 0xA2},  // Ф → 0x94
  {0x0425, 0x58},  // Х → 0x95
  {0x0426, 0x89},  // Ц → 0x96
  {0x0427, 0x8A},  // Ч → 0x97
  {0x0428, 0x8B},  // Ш → 0x98
  {0x0429, 0x8C},  // Щ → 0x99
  {0x042A, 0x8D},  // Ъ → 0x9A
  {0x042B, 0x8E},  // Ы → 0x9B
  {0x042C, 0x62},  // Ь → 0x9C
  {0x042D, 0x8F},  // Э → 0x9D
  {0x042E, 0xAC},  // Ю → 0x9E
  {0x042F, 0xAD},  // Я → 0x9F
  {0x0430, 0x61},  // а → 0xA0
  {0x0431, 0x36},  // б → 0xA1
  {0x0432, 0xDF},  // в → 0xA2
  {0x0433, 0x72},  // г → 0xA3
  {0x0434, 0x67},  // д → 0xA4
  {0x0435, 0x65},  // е → 0xA5
  {0x0436, 0x2A},  // ж → 0xA6
  {0x0437, 0x83},  // з → 0xA7
  {0x0438, 0x75},  // и → 0xA8
  {0x0439, 0xFC},  // й → 0xA9
  {0x043A, 0x6B},  // к → 0xAA
  {0x043B, 0x86},  // л → 0xAB
  {0x043C, 0x6D},  // м → 0xAC
  {0x043D, 0x48},  // н → 0xAD
  {0x043E, 0x6F},  // о → 0xAE
  {0x043F, 0x6E},  // п → 0xAF
  {0x0440, 0x70},  // р → 0xE0
  {0x0441, 0x63},  // с → 0xE1
  {0x0442, 0x54},  // т → 0xE2
  {0x0443, 0x79},  // у → 0xE3
  {0x0444, 0xD8},  // ф → 0xE4
  {0x0445, 0x78},  // х → 0xE5
  {0x0446, 0x75},  // ц → 0xE6
  {0x0447, 0x8A},  // ч → 0xE7
  {0x0448, 0x8B},  // ш → 0xE8
  {0x0449, 0x8C},  // щ → 0xE9
  {0x044A, 0x8D},  // ъ → 0xEA
  {0x044B, 0x8E},  // ы → 0xEB
  {0x044C, 0x62},  // ь → 0xEC
  {0x044D, 0x8F},  // э → 0xED
  {0x044E, 0xAC},  // ю → 0xEE
  {0x044F, 0xAD},  // я → 0xEF
  {0x0020, 0x20}   // пробел → 0x20
};

static const uint8_t rusCharMapSize = sizeof(rusCharMap) / sizeof(rusCharMap[0]);

// Конструктор: сохраняем номера пинов и копируем массив
FutabaCyrillicLCD::FutabaCyrillicLCD(uint8_t rs, uint8_t rw, uint8_t e, const uint8_t dataPins[8])
  : _rs(rs), _rw(rw), _e(e)
{
  memcpy(_dataPins, dataPins, 8);
}

// begin(): настройка пинов и базовых режимов дисплея
void FutabaCyrillicLCD::begin() {
  // настраиваем вывод/ввод
  pinMode(_rs, OUTPUT);
  pinMode(_rw, OUTPUT);
  pinMode(_e, OUTPUT);
  for (uint8_t i=0; i<8; i++) pinMode(_dataPins[i], OUTPUT);

  // после питания обязательно ждём >40мс
  delay(50);
  clear();                              // CMD 0x01
  functionSet(true,true,false);         // 8бит,2строки,5×8
  setDisplay(true,false,false);         // дисплей вкл, курсор/мигание выкл
  setEntryMode(true,false);             // инкремент курсора вправо
}

// clear(): команда 0x01 – очистка экрана и сброс DDRAM
void FutabaCyrillicLCD::clear() {
  send(0x01, false);    // CMD=Clear Display
  waitBusy();           // ждём BF=0 вместо delay(2ms)
}

// cursorHome(): команда 0x02 – вернуть курсор в (0,0)
void FutabaCyrillicLCD::cursorHome() {
  send(0x02, false);    // CMD=Return Home
  waitBusy();           // ждём BF=0
}

// setEntryMode(): команда 0x04 | I/D | S
void FutabaCyrillicLCD::setEntryMode(bool inc, bool shiftDisplay) {
  uint8_t cmd = 0x04 
	| (inc?0x02:0) 
	| (shiftDisplay?0x01:0);
  send(cmd, false);
}

// setDisplay(): команда 0x08 | D | C | B
void FutabaCyrillicLCD::setDisplay(bool on, bool cursorOn, bool blinkOn) {
  uint8_t cmd = 0x08
    | (on       ? 0x04 : 0x00)
    | (cursorOn ? 0x02 : 0x00)
    | (blinkOn  ? 0x01 : 0x00);
  send(cmd, false);
}

// shift(): команда 0x10 | S/C | R/L
void FutabaCyrillicLCD::shift(bool displayShift, bool toRight) {
  uint8_t cmd = 0x10
    | (displayShift ? 0x08 : 0x00)
    | (toRight      ? 0x04 : 0x00);
  send(cmd, false);
}

// functionSet(): команда 0x20 | DL | N | F
void FutabaCyrillicLCD::functionSet(bool data8bit, bool twoLines, bool font5x10) {
  uint8_t cmd = 0x20
    | (data8bit  ? 0x10 : 0x00)
    | (twoLines  ? 0x08 : 0x00)
    | (font5x10  ? 0x04 : 0x00);
  send(cmd, false);
}

// setCGRAMAddr(): команда 0x40 + A5..A0
void FutabaCyrillicLCD::setCGRAMAddr(uint8_t addr) {
  send(0x40 | (addr & 0x3F), false);
}

// setDDRAMAddr(): команда 0x80 + A6..A0
void FutabaCyrillicLCD::setDDRAMAddr(uint8_t addr) {
  send(0x80 | (addr & 0x7F), false);
}

// setCursor(): вычисление DDRAM-адреса от колонки/строки
void FutabaCyrillicLCD::setCursor(uint8_t col, uint8_t row) {
  static const uint8_t rowOffsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if (row > 3) row = 3;                   // максимум – 4-я строка
  uint8_t addr = rowOffsets[row] + col; 
  setDDRAMAddr(addr);
}

// print(): отправка ASCII-строки без обработки
void FutabaCyrillicLCD::print(const String &text) {
  for (uint16_t i = 0; i < text.length(); i++)
    send(text[i], true);
}

// printRus(): отправка UTF-8 строки с конверсией кириллицы
void FutabaCyrillicLCD::printRus(const char *utf8) {
  const char *p = utf8;
  while (*p) {
    uint16_t cp   = decodeUTF8(p);  // Unicode codepoint
    uint8_t code  = mapRus(cp);     // Код дисплея
    send(code, true);               // Вывод символа
  }
}

// writeChar(): вывод одного байта-символа
void FutabaCyrillicLCD::writeChar(uint8_t chr) {
  send(chr, true);
}

// createChar(): загрузка пользовательского символа в CGRAM
void FutabaCyrillicLCD::createChar(uint8_t location, uint8_t charmap[8]) {
  location &= 0x07;          // Оставляем 3 младших бита
  setCGRAMAddr(location << 3);// Перемещаемся в нужный блок CGRAM
  for (uint8_t i = 0; i < 8; i++)
    send(charmap[i], true);  // Запись каждого байта шаблона
  setDDRAMAddr(0);           // Возврат в DDRAM
}

// send(): обёртка для отправки команды/данных
void FutabaCyrillicLCD::send(uint8_t value, bool isData) {
  // RS=1→данные, RS=0→команда
  digitalWrite(_rs, isData? HIGH:LOW);
  // RW=0→запись
  digitalWrite(_rw, LOW);
  writeByte(value);
  pulseEnable();
  // после каждого байта можно ждать BF, но для скорости пропускаем —
  // ждём только там, где нужно (clear/home)
}

void FutabaCyrillicLCD::waitBusy() {
  // Переключаем D7 на ввод, RW=1, RS=0 и читаем BF, пока 1
  pinMode(_dataPins[7], INPUT);
  digitalWrite(_rs, LOW);   // команда
  digitalWrite(_rw, HIGH);  // чтение
  uint8_t busy;
  do {
    digitalWrite(_e, LOW);
    delayMicroseconds(1);
    digitalWrite(_e, HIGH);
    delayMicroseconds(1);
    busy = digitalRead(_dataPins[7]); // D7 = BF
    digitalWrite(_e, LOW);
    delayMicroseconds(1);
  } while (busy);
  // возвращаем D7..D0 назад на вывод
  for (uint8_t i=0; i<8; i++)
    pinMode(_dataPins[i], OUTPUT);
}

uint8_t FutabaCyrillicLCD::readByte() {
  uint8_t val = 0;
  digitalWrite(_rw, HIGH);
  digitalWrite(_rs, LOW);
  for (uint8_t i=0; i<8; i++) {
    digitalWrite(_e, HIGH);
    delayMicroseconds(1);
    if (digitalRead(_dataPins[i])) val |= (1<<i);
    digitalWrite(_e, LOW);
    delayMicroseconds(1);
  }
  return val;
}

// writeByte(): прямое управление линиями данных D0..D7
void FutabaCyrillicLCD::writeByte(uint8_t value) {
  // выставляем все D0..D7
  for (uint8_t i=0; i<8; i++)
    digitalWrite(_dataPins[i], (value>>i)&0x01);
}

// pulseEnable(): формирование сигнала выборки/записи
void FutabaCyrillicLCD::pulseEnable() {
  digitalWrite(_e, LOW);           // Ставим E в 0
  delayMicroseconds(1);            // Минимум 450ns
  digitalWrite(_e, HIGH);          // Ставим E в 1 (пик)
  delayMicroseconds(1);            // Минимум 450ns удержания
  digitalWrite(_e, LOW);           // Возвращаем E в 0
  delayMicroseconds(100);          // Завершающая задержка
}

// decodeUTF8(): простой декодер для ASCII и кириллицы
uint16_t FutabaCyrillicLCD::decodeUTF8(const char *&s) {
  uint8_t c = (uint8_t)*s;
  if ((c & 0x80) == 0) {
    // 1-байтовая ASCII
    s++;
    return c;
  }
  if ((c & 0xE0) == 0xC0) {
    // 2-байтовая последовательность (кириллица)
    uint16_t hi = c & 0x1F;           // старшие 5 бит
    uint16_t lo = (uint8_t)s[1] & 0x3F; // младшие 6 бит
    s += 2;
    return (hi << 6) | lo;            // соединяем
  }
  // Прочие – считаем неизвестным
  s++;
  return '?';
}

// mapRus(): поиск Unicode в таблице и возврат кода дисплея
uint8_t FutabaCyrillicLCD::mapRus(uint16_t codepoint) {

 for (uint8_t i = 0; i < rusCharMapSize; i++) {
   if (rusCharMap[i].unicode == codepoint)
     return rusCharMap[i].lcdCode;
 }
 // fallback: если это обычный ASCII-символ (латиница, цифры, знаки),
 // выводим его как есть
 if (codepoint <= 0x7F)
   return static_cast<uint8_t>(codepoint);
 // во всех остальных случаях — вопросительный знак
 return '?';
}
