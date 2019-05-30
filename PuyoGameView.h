#ifndef PUYO_GAME_VIEW_H
#define PUYO_GAME_VIEW_H

#include <curses.h>
#include "PuyoGameController.h"

class PuyoView
{
public:
    PuyoView(PuyoArrayActive &puyo_active_, PuyoArrayStack &puyo_stack_, PuyoControl &control_);
    ~PuyoView();

    void Display();
    bool ShowPuyo(void);
    bool ShowStats(void);
    void PrintPuyo(unsigned int y, unsigned int x, puyocolor puyo_print);
    bool GetAnimationState();
    void VanishAnimation(bool* puyo_temp_dfs);
    int delay;
    const int waitCount; //1000; //for debug
private:
    PuyoArrayActive &puyo_active;
    PuyoArrayStack &puyo_stack;
    PuyoControl &control;
    bool animation_state;
};

#endif