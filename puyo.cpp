#include <curses.h>
#include <ctime>
#include <cstdlib>

//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor { NONE, RED, BLUE, GREEN, YELLOW };

//0426 PuyoArrayクラスを追加
class PuyoArray
{


}

void PuyoArray::Release()
{
}

void PuyoArray::ChangeSize(unsigned int line, unsigned int column)
{

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


void PuyoArray::RandomInit(void){
	srand((unsigned int) time(NULL));
	for (int i=0;i<5;i++)rand();//カラ実行　
}
	
//盤面に新しいぷよ生成
void PuyoArray::GeneratePuyo()
{

	puyocolor newpuyo1;
	newpuyo1 = (puyocolor) (rand()%4+1);

	puyocolor newpuyo2;
	newpuyo2 = (puyocolor) (rand()%4+1);

	SetValue(0, 5, newpuyo1);
	SetValue(0, 6, newpuyo2);
}


