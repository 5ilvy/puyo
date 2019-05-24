//PuyoGameView.cpp
#include "PuyoGameView.h"

PuyoView::PuyoView(PuyoArrayActive &puyo_active_, PuyoArrayStack &puyo_stack_, PuyoControl &control_) 
         : delay(0), waitCount(3000), puyo_active (puyo_active_), puyo_stack(puyo_stack_), control(control_)
{
    //画面の初期化
    initscr();
    //カラー属性を扱うための初期化
    start_color();

    //カラーパレットの初期化(FG)
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);

    //カラーパレットの初期化(BG)
    init_pair(RED_BG, COLOR_WHITE, COLOR_RED);
    init_pair(BLUE_BG, COLOR_WHITE, COLOR_BLUE);
    init_pair(GREEN_BG, COLOR_WHITE, COLOR_GREEN);
    init_pair(YELLOW_BG, COLOR_BLACK, COLOR_YELLOW);
	init_pair(WHITE_BG, COLOR_BLACK, COLOR_WHITE);

    //キーを押しても画面に表示しない
    noecho();

    //キー入力を即座に受け付ける
    cbreak();

    curs_set(0);

    //キー入力受付方法指定
    keypad(stdscr, TRUE);

    //キー入力非ブロッキングモード
    timeout(0);

	animation_state = false;
}

PuyoView::~PuyoView(void)
{
    //画面をリセット
	endwin();
}
bool PuyoView::GetAnimationState(){
	return animation_state;
}
void PuyoView::Display()
{
    ShowPuyo();
    ShowStats();

	refresh();
    delay++;
}
bool PuyoView::ShowStats(void)
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
	sprintf(msg,"NEXT:");
	mvaddstr(8, COLS - 20, msg);
	attrset(COLOR_PAIR(WHITE_BG));
	sprintf(msg,"AFTER NEXT:");
	mvaddstr(10, COLS - 20, msg);

	std::queue<puyocolor> stack_queue;
	control.GetQueue(stack_queue);
	for (int i=0; i<stack_queue.size();i++)
	{
		PrintPuyo(8+2*(i/2), COLS - 8 - !(i%2), stack_queue.front());
		stack_queue.pop();

	}
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
bool PuyoView::ShowPuyo(void)
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
		}
	}
}