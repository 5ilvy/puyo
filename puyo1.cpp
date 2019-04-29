#include <curses.h>
#include <ctime>
#include <cstdlib>

//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor { NONE, RED, BLUE, GREEN, YELLOW };

class PuyoArray {
public:
	PuyoArray(){
		data = NULL;
		data_line = 0;
		data_column = 0;
	}
	~PuyoArray(){
		Release();
	}
	void ChangeSize(unsigned int line, unsigned int column);
	unsigned int GetLine();
	unsigned int GetColumn();
	puyocolor GetValue(unsigned int y, unsigned int x);
	void SetValue(unsigned int y, unsigned int x, puyocolor data);
private:
	//盤面状態
	puyocolor *data;
	//盤面の行数，列数
	unsigned int data_line;
	unsigned int data_column;

	void Release();

};
//メモリ開放
void PuyoArray::Release()
{
	if (data == NULL) {
		return;
	}
	delete[] data;
	data = NULL;
}

//盤面サイズ変更
void PuyoArray::ChangeSize(unsigned int line, unsigned int column)
{
	Release();

	//新しいサイズでメモリ確保
	data = new puyocolor[line*column];

	data_line = line;
	data_column = column;
}

//盤面の行数を返す
unsigned int PuyoArray::GetLine(){
	return data_line;
}

//盤面の列数を返す
unsigned int PuyoArray::GetColumn()
{
	return data_column;
}

//盤面の指定された位置の値を返す
puyocolor PuyoArray::GetValue(unsigned int y, unsigned int x)
{
	if (y >= GetLine() || x >= GetColumn())
	{
		//引数の値が正しくない
		return NONE;
	}

	return data[y*GetColumn() + x];
}

//盤面の指定された位置に値を書き込む
void PuyoArray::SetValue(unsigned int y, unsigned int x, puyocolor value)
{
	if (y >= GetLine() || x >= GetColumn())
	{
		//引数の値が正しくない
		return;
	}

	data[y*GetColumn() + x] = value;
}

class PuyoControl{
public:
	void GeneratePuyo(PuyoArray &puyo);
	bool LandingPuyo(PuyoArray &puyo);
	void MoveLeft(PuyoArray &puyo);
	void MoveRight(PuyoArray &puyo);
	void MoveDown(PuyoArray &puyo);
};



	
//盤面に新しいぷよ生成
void PuyoControl::GeneratePuyo(PuyoArray &puyo)
{

	puyocolor newpuyo1;
	newpuyo1 = (puyocolor) (rand()%4+1);

	puyocolor newpuyo2;
	newpuyo2 = (puyocolor) (rand()%4+1);

	puyo.SetValue(0, 5, newpuyo1);
	puyo.SetValue(0, 6, newpuyo2);
}

//ぷよの着地判定．着地判定があるとtrueを返す
bool PuyoControl::LandingPuyo(PuyoArray &puyo)
{
	bool landed = false;

	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			if (puyo.GetValue(y, x) != NONE && y == puyo.GetLine() - 1)
			{
				landed = true;

				//着地判定されたぷよを消す．本処理は必要に応じて変更する．
				puyo.SetValue(y, x, NONE);
			}
		}
	}

	return landed;
}

//左移動
void PuyoControl::MoveLeft(PuyoArray &puyo)
{
	//一時的格納場所メモリ確保
	puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];

	for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++)
	{
		puyo_temp[i] = NONE;
	}

	//1つ左の位置にpuyoactiveからpuyo_tempへとコピー
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			if (puyo.GetValue(y, x) == NONE) {
				continue;
			}

			if (0 < x && puyo.GetValue(y, x - 1) == NONE)
			{
				puyo_temp[y*puyo.GetColumn() + (x - 1)] = puyo.GetValue(y, x);
				//コピー後に元位置のpuyoactiveのデータは消す
				puyo.SetValue(y, x, NONE);
			}
			else
			{
				puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
			}
		}
	}

	//puyo_tempからpuyoactiveへコピー
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
		}
	}

	//一時的格納場所メモリ解放
	delete[] puyo_temp;
}

//右移動
void PuyoControl::MoveRight(PuyoArray &puyo)
{
	//一時的格納場所メモリ確保
	puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];

	for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++)
	{
		puyo_temp[i] = NONE;
	}

	//1つ右の位置にpuyoactiveからpuyo_tempへとコピー
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = puyo.GetColumn() - 1; x >= 0; x--)
		{
			if (puyo.GetValue(y, x) == NONE) {
				continue;
			}

			if (x < puyo.GetColumn() - 1 && puyo.GetValue(y, x + 1) == NONE)
			{
				puyo_temp[y*puyo.GetColumn() + (x + 1)] = puyo.GetValue(y, x);
				//コピー後に元位置のpuyoactiveのデータは消す
				puyo.SetValue(y, x, NONE);
			}
			else
			{
				puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
			}
		}
	}

	//puyo_tempからpuyoactiveへコピー
	for (int y = 0; y <puyo.GetLine(); y++)
	{
		for (int x = 0; x <puyo.GetColumn(); x++)
		{
			puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
		}
	}

	//一時的格納場所メモリ解放
	delete[] puyo_temp;
}

//下移動
void PuyoControl::MoveDown(PuyoArray &puyo)
{
	//一時的格納場所メモリ確保
	puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];

	for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++)
	{
		puyo_temp[i] = NONE;
	}

	//1つ下の位置にpuyoactiveからpuyo_tempへとコピー
	for (int y = puyo.GetLine() - 1; y >= 0; y--)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			if (puyo.GetValue(y, x) == NONE) {
				continue;
			}

			if (y < puyo.GetLine() - 1 && puyo.GetValue(y + 1, x) == NONE)
			{
				puyo_temp[(y + 1)*puyo.GetColumn() + x] = puyo.GetValue(y, x);
				//コピー後に元位置のpuyoactiveのデータは消す
				puyo.SetValue(y, x, NONE);
			}
			else
			{
				puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
			}
		}
	}

	//puyo_tempからpuyoactiveへコピー
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
		}
	}

	//一時的格納場所メモリ解放
	delete[] puyo_temp;
}


//表示
void Display(PuyoArray &puyo)
{
	//落下中ぷよ表示
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			switch (puyo.GetValue(y, x))
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
	}


	//情報表示
	int count = 0;
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			if (puyo.GetValue(y, x) != NONE)
			{
				count++;
			}
		}
	}

	char msg[256];
	sprintf(msg, "Field: %d x %d, Puyo number: %03d", puyo.GetLine(), puyo.GetColumn(), count);
	mvaddstr(2, COLS - 35, msg);

	refresh();
}

void color_init(){
	init_pair(RED, COLOR_RED, COLOR_BLACK);
	init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
	
	
}
//ここから実行される
int main(int argc, char **argv){
	//画面の初期化
	initscr();
	//カラー属性を扱うための初期化
	start_color();
	color_init();
	//乱数初期化
	srand((unsigned int) time(NULL));
	for (int i=0;i<5;i++)rand();//カラ実行
	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);


	//初期化処理
	PuyoArray puyo;
	PuyoControl control;

	

	puyo.ChangeSize(LINES/2, COLS/2);	//フィールドは画面サイズの縦横1/2にする
	control.GeneratePuyo(puyo);	//最初のぷよ生成

	int delay = 0;
	int waitCount = 20000;

	int puyostate = 0;


	//メイン処理ループ
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'Q')
		{
			break;
		}

		//入力キーごとの処理
		switch (ch)
		{
		case KEY_LEFT:
			control.MoveLeft(puyo);
			break;
		case KEY_RIGHT:
			control.MoveRight(puyo);
			break;
		case 'z':
			//ぷよ回転処理
			break;
		default:
			break;
		}


		//処理速度調整のためのif文
		if (delay%waitCount == 0){
			//ぷよ下に移動
			control.MoveDown(puyo);
			
			//ぷよ着地判定
			if (control.LandingPuyo(puyo))
			{
				//着地していたら新しいぷよ生成
				control.GeneratePuyo(puyo);
			}
		}
		delay++;

		//表示
		Display(puyo);
	}


	//画面をリセット
	endwin();

	return 0;
}
