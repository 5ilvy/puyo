#ifndef PUYO_GAME_VIEW_H
#define PUYO_GAME_VIEW_H

#include <curses.h>
#include "PGController.h"

class PuyoView
{
public:
    // PuyoView(PuyoArrayActive &puyo_active_, PuyoArrayStack &puyo_stack_, PuyoControl &control_);
    PuyoView();
    ~PuyoView();

    void Display(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack, PuyoControl &control);
    bool ShowPuyo(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack);
    bool ShowStats(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack, PuyoControl &control);
    void PrintPuyo(unsigned int y, unsigned int x, puyocolor puyo_print);
    void ShowCombo(int combos);
    // bool GetAnimationState();
    void GameTitleAnimate();
    void GameTitleInit();
    void VanishAnimation(PuyoArrayActive &puyo_active, bool* puyo_temp_dfs);
    void GameOverModal(int score);
    void GameOverMenu(unsigned int index);
    void GameOverMenuSelected(unsigned int index);
    int delay;
    const int waitCount; //1000; //for debug
private:

};

#endif