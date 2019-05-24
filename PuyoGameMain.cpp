#include "PuyoGameController.h"
#include "PuyoGameView.h"
//ここから実行される

int main(int argc, char **argv)
{
	//初期化処理
	PuyoArrayActive puyo_active;
	PuyoArrayStack puyo_stack;
	PuyoControl control(puyo_active, puyo_stack);
	PuyoView view(puyo_active, puyo_stack, control);



	puyo_active.ChangeSize(LINES / 2, COLS / 2);
	puyo_stack.ChangeSize(LINES / 2, COLS / 2); //フィールドは画面サイズの縦横1/2にする
	control.GeneratePuyo();
	control.PopFromQueue();			//最初のぷよ生成

	// int puyostate = 0;

			//メイン処理ループ
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'q' ||ch == 'Q' )
		{
			break;
		}

		//入力キーごとの処理
		switch (ch)
		{
		case KEY_LEFT:
			control.MoveLeft();
			break;
		case KEY_RIGHT:
			control.MoveRight();
			break;
		case 'z':
			//ぷよ回転処理
			control.RotateCw();
			// バグが治らないので今回は見送り
			break;
		default:
			break;
		}
		


		//処理速度調整のためのif文
		if (view.delay % view.waitCount == 0)
		{
			//ぷよ下に移動
			control.MoveDown();
			//ぷよ着地判定
			if (control.isLandingPuyo())
			{
				puyo_stack.VanishPuyo();
				//着地していたら新しいぷよ生成
				control.PopFromQueue();
			}
			

		}
				//VanishAnimation(未実装)表示中でなければ実行
		if(!view.GetAnimationState())
			control.MoveStackDown();
		//表示
		view.Display();
	}

	return 0;
}