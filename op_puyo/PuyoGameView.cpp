//PuyoGameView.cpp
#include "PuyoGameView.h"

PuyoView::PuyoView()
	: delay(0), waitCount(3000)
{
}

PuyoView::~PuyoView(void)
{
	//画面をリセット
	endwin();
}
// bool PuyoView::GetAnimationState(){
// 	return animation_state;
// }

void PuyoView::VanishAnimation(PuyoArrayActive &puyo_active, bool *puyo_temp_dfs)
{
	for (int y_ = 0; y_ < puyo_active.GetLine(); y_++)
	{
		for (int x_ = 0; x_ < puyo_active.GetColumn(); x_++)
		{
			if (puyo_temp_dfs[y_ * puyo_active.GetColumn() + x_])
				PrintPuyo(y_, x_, NONE); //暫定
		}
	}
}

void PuyoView::Display(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack, PuyoControl &control)
{
	ShowPuyo(puyo_active, puyo_stack);
	ShowStats(puyo_active, puyo_stack, control);

	refresh();
	delay++;
}
bool PuyoView::ShowStats(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack, PuyoControl &control)
{
	//情報表示
	int count = puyo_active.GetCount() + puyo_stack.GetCount();

	char msg[256];
	// attrset(COLOR_PAIR(RED_BG));
	attrset(COLOR_PAIR(WHITE_BG));
	sprintf(msg, "--------------------");
	mvaddstr(1, COLS - 20, msg);
	sprintf(msg, "| Field: %03d x %03d |", puyo_active.GetLine(), puyo_active.GetColumn());
	mvaddstr(2, COLS - 20, msg);
	sprintf(msg, "| Puyo number: %03d |", count);
	mvaddstr(3, COLS - 20, msg);
	//sprintf(msg,"|Chaining: %03d   |", puyo_stack.GetChainNum());
	//mvaddstr(4, COLS - 45, msg);
	sprintf(msg, "--------------------");
	mvaddstr(4, COLS - 20, msg);

	//次のぷよを表示
	attrset(COLOR_PAIR(WHITE_BG));
	sprintf(msg, "NEXT:");
	mvaddstr(8, COLS - 20, msg);
	attrset(COLOR_PAIR(WHITE_BG));
	sprintf(msg, "AFTER NEXT:");
	mvaddstr(10, COLS - 20, msg);

	std::queue<puyocolor> stack_queue;
	control.GetQueue(stack_queue);
	for (int i = 0; i < stack_queue.size(); i++)
	{
		PrintPuyo(8 + 2 * (i / 2), COLS - 8 - !(i % 2), stack_queue.front());
		stack_queue.pop();
	}
	return true;
}

void PuyoView::PrintPuyo(unsigned int y, unsigned int x, puyocolor puyo_print)
{
	switch (puyo_print)
	{
	case NONE:
		attrset(COLOR_PAIR(0));
		mvaddch(y, x, '.');
		break;
	case RED:
		attrset(COLOR_PAIR(RED));
		mvaddch(y, x, 'R');
		break;
	case BLUE:
		attrset(COLOR_PAIR(BLUE));
		mvaddch(y, x, 'B');
		break;
	case GREEN:
		attrset(COLOR_PAIR(GREEN));
		mvaddch(y, x, 'G');
		break;
	case YELLOW:
		attrset(COLOR_PAIR(YELLOW));
		mvaddch(y, x, 'Y');
		break;
	default:
		attrset(COLOR_PAIR(0));
		mvaddch(y, x, '?');
		break;
	}
}
bool PuyoView::ShowPuyo(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack)
{
	for (int y = 0; y < puyo_active.GetLine(); y++)
	{
		for (int x = 0; x < puyo_active.GetColumn(); x++)
		{
			//落下中ぷよ表示
			PrintPuyo(y, x, puyo_active.GetValue(y, x));

			//落下済みぷよ表示
			switch (puyo_stack.GetValue(y, x))
			{
			case NONE:
				//何も描画しない
				break;
			case RED:
				attrset(COLOR_PAIR(RED));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
				attrset(COLOR_PAIR(BLUE));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
				attrset(COLOR_PAIR(GREEN));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
				attrset(COLOR_PAIR(YELLOW));
				mvaddch(y, x, 'Y');
				break;
			case VANISH:
				attrset(COLOR_PAIR(0));
				mvaddch(y, x, ' ');
			default:
				break;
			}
			// if (puyo_active.GetValue(y,x)!=NONE && puyo_stack.GetValue(y,x)!=NONE)
			// mvaddch(y, x, '?');
		}
	}
	return true;
}
void PuyoView::GameTitleAnimate()
{
	// アニメーション用の点滅管理
	static int flash_delay = 0;
	refresh();
	// アニメーション
	{
		char msg[256];
		attrset(COLOR_PAIR(WHITE_BG));
		if ((flash_delay++ % 100000) > 50000)
		{
			if (COLS >= 80)
			{
				sprintf(msg, "                      >  PRESS SPACE KEY TO START  <                            ");
				mvaddstr(LINES / 2 + 4, COLS / 2 - 40, msg);
			}
			else
			{
				sprintf(msg, "> PRESS SPACE KEY TO START <");
				mvaddstr(7, 5, msg);
			}
		}
		else
		{
			if (COLS >= 80)
			{
				sprintf(msg, "                      >                            <                            ");
				mvaddstr(LINES / 2 + 4, COLS / 2 - 40, msg);
			}
			else
			{
				sprintf(msg, ">                          <");
				mvaddstr(7, 5, msg);
			}
		}
	}
	if (flash_delay >= 100000)
		flash_delay = 0;
}
void PuyoView::GameTitleInit()
{
	//title画面の表示
	char msg[256];
	attrset(COLOR_PAIR(NONE_BG));
	for (int y = 0; y < LINES / 2 - 5; y++)
	{
		refresh();
		for (int ay = 0; ay < LINES; ay++)
		{
			for (int ax = 0; ax < COLS; ax++)
			{
				mvaddch(ay, ax, ' ');
			}
			usleep(1000);
		}
		attrset(COLOR_PAIR(RED));
		if (COLS >= 80)
		{
			sprintf(msg, " ######                          #     #                                        ");
			mvaddstr(y, COLS / 2 - 40, msg);
			sprintf(msg, " #     #  #    #   #   #   ####  ##   ##   ####   #####    ####   #    #     #  ");
			mvaddstr(y + 1, COLS / 2 - 40, msg);
			sprintf(msg, " #     #  #    #    # #   #    # # # # #  #    #  #    #  #    #  #   #      #  ");
			mvaddstr(y + 2, COLS / 2 - 40, msg);
			sprintf(msg, " ######   #    #     #    #    # #  #  #  #    #  #    #  #    #  ####       #  ");
			mvaddstr(y + 3, COLS / 2 - 40, msg);
			sprintf(msg, " #        #    #     #    #    # #     #  #    #  #    #  #    #  #  #       #  ");
			mvaddstr(y + 4, COLS / 2 - 40, msg);
			sprintf(msg, " #        #    #     #    #    # #     #  #    #  #    #  #    #  #   #      #  ");
			mvaddstr(y + 5, COLS / 2 - 40, msg);
			sprintf(msg, " #         ####      #     ####  #     #   ####   #####    ####   #    #     #  ");
			mvaddstr(y + 6, COLS / 2 - 40, msg);
			sprintf(msg, "                                                                           2019 ");
			mvaddstr(y + 7, COLS / 2 - 40, msg);
			sprintf(msg, "                      >  PRESS SPACE KEY TO START  <                            ");
			mvaddstr(y + 10, COLS / 2 - 40, msg);
		}
		else
		{
			sprintf(msg, "============================");
			mvaddstr(2, 5, msg);
			sprintf(msg, " PuyoModoki");
			mvaddstr(3, 5, msg);
			sprintf(msg, "============================");
			mvaddstr(4, 5, msg);
			sprintf(msg, "  made by            , 2019 ");
			mvaddstr(5, 5, msg);
			sprintf(msg, "> PRESS SPACE KEY TO START <");
			mvaddstr(7, 5, msg);
		}
		usleep(3000);
	}
}

void PuyoView::GameOverModal(int score)
{
	initscr();
	char msg[256];
	attrset(COLOR_PAIR(NONE_BG));
	for (int y = 0; y < LINES / 2- 6; y++)
	{
		refresh();
		for (int ay = 0; ay < LINES; ay++)
		{
			for (int ax = 0; ax < COLS; ax++)
			{
				mvaddch(ay, ax, ' ');
			}
			usleep(2000);
		}

		
		if (COLS >= 80)
		{
			attrset(COLOR_PAIR(RED));
			sprintf(msg, "  ####                                                           ");
			mvaddstr(y, COLS / 2 - 32, msg);
			sprintf(msg, " #     #    ##    #    #  ######   ####   #    #  ######  #####  ");
			mvaddstr(y + 1, COLS / 2 - 32, msg);
			sprintf(msg, " #         #  #   ##  ##  #       #    #  #    #  #       #    # ");
			mvaddstr(y + 2, COLS / 2 - 32, msg);
			sprintf(msg, " #  ####  #    #  # ## #  #####   #    #  #    #  #####   #    # ");
			mvaddstr(y + 3, COLS / 2 - 32, msg);
			sprintf(msg, " #     #  ######  #    #  #       #    #  #    #  #       #####  ");
			mvaddstr(y + 4, COLS / 2 - 32, msg);
			sprintf(msg, " #     #  #    #  #    #  #       #    #   #  #   #       #   #  ");
			mvaddstr(y + 5, COLS / 2 - 32, msg);
			sprintf(msg, "  #####   #    #  #    #  ######   ####     ##    ######  #    # ");
			mvaddstr(y + 6, COLS / 2 - 32, msg);
			attrset(COLOR_PAIR(NONE));
			sprintf(msg, "YOUR SCORE: %08d",score);
			mvaddstr(y + 7, COLS / 2 - 10, msg);
			sprintf(msg, "                                                                 ");
			mvaddstr(y + 8, COLS / 2 - 10, msg);
			// sprintf(msg, "   RETRY");
			// mvaddstr(y + 10, COLS / 2 - 4, msg);
			// sprintf(msg, "   QUIT ");
			// mvaddstr(y + 11, COLS / 2 - 4, msg);
		}
		else
		{
			attrset(COLOR_PAIR(RED));
			sprintf(msg, "===================");
			mvaddstr(2, 5, msg);
			sprintf(msg, "  G A M E O V E R");
			mvaddstr(3, 5, msg);
			sprintf(msg, "===================");
			mvaddstr(4, 5, msg);
			attrset(COLOR_PAIR(NONE));
			sprintf(msg, " SCORE : %08d", score);
			mvaddstr(5, 5, msg);

		}
	}
	
}

void PuyoView::GameOverMenu(unsigned int index){
	char msg[256];
	if (COLS >= 80)
	{
		attrset(COLOR_PAIR(NONE));
		sprintf(msg, "     RETRY");
		mvaddstr(LINES/2 + 3, COLS / 2 - 5, msg);
		sprintf(msg, "     QUIT ");
		mvaddstr(LINES/2 + 4, COLS / 2 - 5, msg);
		attrset(COLOR_PAIR(RED));
		sprintf(msg, "->");
		mvaddstr(LINES/2 + 3 + index, COLS / 2 - 5, msg);
	}
	else
	{
		attrset(COLOR_PAIR(NONE));
		sprintf(msg, "     RETRY");
		mvaddstr(8, 5, msg);
		sprintf(msg, "     EXIT ");
		mvaddstr(9, 5, msg);
		attrset(COLOR_PAIR(BLUE));
		sprintf(msg, "->");
		mvaddstr(8+index,  5, msg);

	}
}

void PuyoView::GameOverMenuSelected(unsigned int index)
{
	// アニメーション用の点滅管理
	int flash_delay = 0;
	while (flash_delay < 160000){
	refresh();
	char msg[256];
	attrset(COLOR_PAIR(WHITE_BG));
	if ((flash_delay++ % 40000) > 20000)
	{
		if (COLS >= 80)
		{
			attrset(COLOR_PAIR((index == 0) ? RED_BG:NONE));
			sprintf(msg, "     RETRY");
			mvaddstr(LINES / 2 + 3, COLS / 2 - 5, msg);
			attrset(COLOR_PAIR((index == 1) ? RED_BG:NONE));
			sprintf(msg, "     QUIT ");
			mvaddstr(LINES / 2 + 4, COLS / 2 - 5, msg);
			attrset(COLOR_PAIR(NONE));
			sprintf(msg, "->");
			mvaddstr(LINES / 2 + 3 + index, COLS / 2 - 5, msg);
			
		}
		else
		{
			attrset(COLOR_PAIR((index == 0) ? WHITE_BG:NONE));
			sprintf(msg, "     RETRY");
			mvaddstr(8, 5, msg);
			attrset(COLOR_PAIR((index == 1) ? WHITE_BG:NONE));
			sprintf(msg, "     EXIT ");
			mvaddstr(9, 5, msg);
			attrset(COLOR_PAIR(BLUE));
			sprintf(msg, "->");
			mvaddstr(8 + index, 5, msg);
		}
	}else{
		if (COLS >= 80)
		{
			attrset(COLOR_PAIR(NONE));
			sprintf(msg, "     RETRY");
			mvaddstr(LINES / 2 + 3, COLS / 2 - 5, msg);
			sprintf(msg, "     QUIT ");
			mvaddstr(LINES / 2 + 4, COLS / 2 - 5, msg);
			attrset(COLOR_PAIR(RED));
			sprintf(msg, "->");
			mvaddstr(LINES / 2 + 3 + index, COLS / 2 - 5, msg);
			
		}
		else
		{
			attrset(COLOR_PAIR(NONE));
			sprintf(msg, "     RETRY");
			mvaddstr(8, 5, msg);
			sprintf(msg, "     EXIT ");
			mvaddstr(9, 5, msg);
			attrset(COLOR_PAIR(BLUE));
			sprintf(msg, "->");
			mvaddstr(8 + index, 5, msg);
		}
	}
	}
}
void PuyoView::ShowCombo(int combos)
{
}