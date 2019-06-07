#include "PuyoGameController.h"
#include "PuyoGameView.h"

//Scene Controllerの代わりにSceneを管理する変数とswitch で対応

class PuyoGame
{
private:
	int gameScene;
	PuyoArrayActive puyo_active;
	PuyoArrayStack puyo_stack;
	PuyoControl& control(PuyoArrayActive, PuyoArrayStack);
	PuyoView& view(PuyoArrayActive, PuyoArrayStack, PuyoControl);
public:
	PuyoGame()
	: gameScene(0)
	{
		PuyoArrayActive puyo_active;
		PuyoArrayStack puyo_stack;
		control = PuyoControl (puyo_active, puyo_stack);
		view = PuyoView (puyo_active, puyo_stack, control);
	}
	void SceneManager();
	void Initialize();
	void PlayMain();
	void GameOver();
	bool isEnd(){return !(gameScene == 10);};

};
void PuyoGame::SceneManager(){
	switch (gameScene){
		case 0:
			Initialize();
			gameScene++;
			break;
		case 1:
			PlayMain();
			break;
		case 2:
			GameOver();
			break;
	};

}
void PuyoGame::Initialize(){
		puyo_active.ChangeSize(LINES / 2, COLS / 2);
		puyo_stack.ChangeSize(LINES / 2, COLS / 2); //フィールドは画面サイズの縦横1/2にする
		control.GeneratePuyo();//最初のぷよ生成
		control.PopFromQueue();//ぷよ取り出し
}

void PuyoGame::PlayMain(){
	// ゲームオーバーならgameSceneを書き換えてループを抜ける
	if (control.GameOverJudge()){
		gameScene = 2;
		return;
	}

	//キー入力受付
	int ch;
	ch = getch();

	//Qの入力で終了
	if (ch == 'q' ||ch == 'Q' ) {
		gameScene = 10;
		return;
	}

	//入力キーごとの処理
	switch (ch){		
		case KEY_LEFT:
			control.MoveLeft();
			break;
		case KEY_RIGHT:
			control.MoveRight();
			break;
		case ' ':
			while (!control.isLandingPuyo()){
				control.MoveDown();
			}
				puyo_stack.VanishPuyo();
				//着地していたら新しいぷよ生成
				control.PopFromQueue();
			break;
		case 'z':
		if (control.isLandingPuyo())
			{
				puyo_stack.VanishPuyo();
				//着地していたら新しいぷよ生成
				control.PopFromQueue();
			}else{
				control.RotateCw();
			}
			//ぷよ回転処理
			
			
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
			//ぷよ消去
			puyo_stack.VanishPuyo();
			//着地していたら新しいぷよ生成
			control.PopFromQueue();
		}
	}

	//VanishAnimation(未実装)表示中でなければ実行
	if(!view.GetAnimationState()) control.MoveStackDown();
	//表示
	view.Display();
}

void PuyoGame::GameOver(){
	view.GameOverModal();
	while(gameScene == 2){
		refresh();
		//キー入力受付
		int ch;
		ch = getch();
		//Qの入力で終了
		if (ch == 'q' ||ch == 'Q' )
			gameScene = 10;
		if (ch == 'r' ||ch == 'R' )
			gameScene = 0;
	}
	
}
int main(int argc, char **argv)
{
	PuyoGame p;
	while (p.isEnd()){
		p.SceneManager();
	}
// {
// 	//0 - Initialize, 1 - PlayMode, 2 - GameOver, 10 - Exit
// 	int gameScene = 0;

// 	while (gameScene == 10)
// 	{
// 	switch (gameScene){
// 	case 0:
// 		//初期化処理
// 		PuyoArrayActive puyo_active;
// 		PuyoArrayStack puyo_stack;
// 		PuyoControl control(puyo_active, puyo_stack);
// 		PuyoView view(puyo_active, puyo_stack, control);

// 		puyo_active.ChangeSize(LINES / 2, COLS / 2);
// 		puyo_stack.ChangeSize(LINES / 2, COLS / 2); //フィールドは画面サイズの縦横1/2にする
// 		control.GeneratePuyo();//最初のぷよ生成
// 		control.PopFromQueue();//ぷよ取り出し

// 		gameScene = 1; //ゲームスタート
// 		break;
// 	case 1:
// 		//メイン処理ループ
// 		while (gameScene == 1)
// 		{
// 			// ゲームオーバーならgameSceneを書き換えてループを抜ける
// 			if (control.GameOverJudge()){
// 				gameScene = 2;
// 				continue;
// 			}

// 			//キー入力受付
// 			int ch;
// 			ch = getch();

// 			//Qの入力で終了
// 			if (ch == 'q' ||ch == 'Q' )
// 			{
// 				break;


// 			}

// 			//入力キーごとの処理
// 			switch (ch){		
// 				case KEY_LEFT:
// 					control.MoveLeft();
// 					break;
// 				case KEY_RIGHT:
// 					control.MoveRight();
// 					break;
// 				case ' ':
// 					while (!control.isLandingPuyo()){
// 						control.MoveDown();
// 					}
// 						puyo_stack.VanishPuyo();
// 						//着地していたら新しいぷよ生成
// 						control.PopFromQueue();
// 					break;
// 				case 'z':
// 				if (control.isLandingPuyo())
// 					{
// 						puyo_stack.VanishPuyo();
// 						//着地していたら新しいぷよ生成
// 						control.PopFromQueue();
// 					}else{
// 						control.RotateCw();
// 					}
// 					//ぷよ回転処理
					
					
// 					break;
// 				default:
// 					break;
// 			}
			
// 			//処理速度調整のためのif文
// 			if (view.delay % view.waitCount == 0)
// 			{
// 				//ぷよ下に移動
// 				control.MoveDown();
// 				//ぷよ着地判定
// 				if (control.isLandingPuyo())
// 				{
// 					//ぷよ消去
// 					puyo_stack.VanishPuyo();
// 					//着地していたら新しいぷよ生成
// 					control.PopFromQueue();
// 				}
				

// 			}
// 			//VanishAnimation(未実装)表示中でなければ実行
// 			if(!view.GetAnimationState())
// 				control.MoveStackDown();
// 			//表示
// 			view.Display();
// 		}
// 		break;
	
	
// 	case 2:
// 		view.GameOverModal();
// 		while(gameScene == 2){
// 			refresh();
// 			//キー入力受付
// 			int ch;
// 			ch = getch();
// 			//Qの入力で終了
// 			if (ch == 'q' ||ch == 'Q' )
// 				gameScene = 10;
// 			if (ch == 'r' ||ch == 'R' )
// 				gameScene = 0;
			
// 			}
// 		}
// 		break;
// 	case 10:
// 		break;
// 	}
	

	return 0;
}