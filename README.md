# RATMM Panel — краткое руководство

Это небольшая документация по новым архитектурным изменениям, введённым в проекте: контракт `Screen`, работа с PROGMEM и рекомендации по экономии RAM.

## Screen контракт
Каждый экран реализует поведение через абстрактный базовый класс `Screen` (в `src/screens/Screen.h`). Рекомендуемые виртуальные методы:

- `void enter()` — вызывается при входе на экран; не выполняйте длительную работу синхронно.
- `void exit()` — очистка/освобождение по выходу.
- `void load()` — загрузить состояние экрана из регистров/`GlobalState`.
- `void save()` — сохранить состояние в регистры/`GlobalState`.
- `void update()` — периодическое обновление (таймеры, опрос датчиков).
- `void render()` — отрисовка на LCD (рисует только текущий экран).
- `void handleKey(char key)` — обработка нажатий.

Контракт — минимальный, но достаточный шаблон:

- `enter()` и `exit()` должны быть быстрыми. Долгая инициализация — в `update()`.
- `load()`/`save()` — идемпотентные и короткие.
- `render()` не должен блокировать долго и должен использовать `GlobalState` для получения данных.

## Как добавить новый экран
1. Создайте заголовок: `src/screens/ScreenExample.h`:

```cpp
#pragma once
#include "screens/Screen.h"

class ScreenExample : public Screen {
public:
  void enter() override;
  void exit() override;
  void load() override;
  void save() override;
  void update() override;
  void render() override;
  void handleKey(char key) override;
};

extern ScreenExample screenExample;
```

2. Реализуйте `src/screens/ScreenExample.cpp` и используйте `gState` для доступа к глобальному состоянию:

```cpp
#include "ScreenExample.h"
#include "GlobalState.h"
#include "utils.h" // для readProgmem* если нужны PROGMEM строки

void ScreenExample::enter() { }
void ScreenExample::exit() { }
void ScreenExample::load() { }
void ScreenExample::save() { }
void ScreenExample::update() { }
void ScreenExample::render() {
  char buf[32];
  readProgmemString(exampleTitle, buf, sizeof(buf));
  lcd.printRus(buf);
}
void ScreenExample::handleKey(char key) { }

ScreenExample screenExample;
```

3. Зарегистрируйте экран в основной логике (`src/main.cpp`) — добавьте соответствие `AppState -> Screen*` и переключение состояний.

## PROGMEM: helpers и рекомендации
- Используйте `readProgmemString(PGM_P src, char *dst, size_t dstSize)` и `readProgmemTableString(const char *const table[] PROGMEM, uint8_t idx, char *dst, size_t dstSize)` из `src/utils.*` для безопасного и единообразного чтения строк из flash.
- Никогда не передавайте указатель на строку в PROGMEM туда, где ожидается обычный указатель на RAM; всегда копируйте в локальный буфер, если библиотека/функция требует RAM-строку.
- Используйте `snprintf` вместо `sprintf` для защиты буферов.

Пример чтения строки из таблицы:

```cpp
char tmp[24];
readProgmemTableString(mainMenuItems, index, tmp, sizeof(tmp));
lcd.printRus(tmp);
```

## Fixed-point (рекомендация для analog-данных)
Чтобы уменьшить использование RAM/Flash и избежать затрат на поддержку плавающей точки на AVR, рекомендуем хранить значения аналоговых настроек в фиксированном формате (scaled integers).

- Подход: хранить значение как `int16_t` — фактически value * 100 (например, 12.34°C → 1234).
- Преимущества: меньше памяти, детерминированные операции, отсутствие FP библиотек.

Примеры вспомогательных функций:

```cpp
// конвертация для отображения
static inline void formatFixed(char *dst, size_t dstSize, int16_t scaled) {
  int whole = scaled / 100;
  int frac = abs(scaled % 100);
  snprintf(dst, dstSize, "%d.%02d", whole, frac);
}

// при чтении/записи регистров — масштабирование
int16_t sensorScaled = RTU.holdingRegisterRead(BASE_ADDR) - OFFSET; // пример
```

Оценка экономии RAM
- Переход с `float` (4 байта) на `int16_t` (2 байта) экономит 2 байта на каждое значение. Для 5 аналоговых входов — экономия ~10 байт только на массиве значений, плюс отсутствие runtime FP tables/stack usage в функциях с плавающей точкой.

Риски
- Необходима проверка точности и границ (overflow при больших значениях). Подберите масштаб (x100, x10) согласно диапазону значений.

## Сборка
В PowerShell:

```powershell
C:\Users\Admin\.platformio\penv\Scripts\platformio.exe run -d "C:\Users\Admin\Documents\PlatformIO\Projects\RATMM Panel"
```

## Дальнейшие шаги
- При желании я могу автоматически: создать `CHANGELOG.md` (выполнено), подготовить PR, пройтись по другим проектам в workspace и унифицировать PROGMEM-хелперы, или завершить полный перевод analog → fixed-point (включая изменения в `GlobalState` и `MenuStructs`).

Если хотите, могу приступить к завершению перевода float→fixed-point: предложу конкретные изменения (файлы/поля), внесу патч и запущу сборку. Скажите, действовать автоматически или сначала показать план?