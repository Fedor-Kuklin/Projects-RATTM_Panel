#pragma once
#include "Screen.h"
#include "../Futaba/FutabaCyrillicLCD.h"
#include "../MenuStructs.h"
#include "../GlobalState.h"

class ScreenEnterPassword : public Screen {
public:
    void enter() override;
    void exit() override;
    void update() override;
    void render() override;
    void load() override;
    void handleKey(char key) override;
private:
    char input[6];
    uint8_t len = 0;
};

extern ScreenEnterPassword screenEnterPassword;
// Удалён устаревший обёртывающий show-функция; используйте screenEnterPassword.render()
