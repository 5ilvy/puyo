#ifndef PUYO_GAME_CONTROLLER_H
#define PUYO_GAME_CONTROLLER_H

#include <curses.h>
#include <ctime>
#include <cstdlib>
#include <queue>


enum puyocolor
{
	NONE,
	RED,
	BLUE,
	GREEN,
	YELLOW,
	VANISH
};
enum puyo_bg_color
{
	NONE_BG = 6,
	RED_BG = 7,
	BLUE_BG = 8,
	GREEN_BG = 9,
	YELLOW_BG = 10,
	WHITE_BG = 11
};

class PuyoArray
{
public:
	PuyoArray();
	~PuyoArray();
	void ChangeSize(unsigned int line, unsigned int column);
	unsigned int GetLine();
	unsigned int GetColumn();
	puyocolor GetValue(unsigned int y, unsigned int x);
	void SetValue(unsigned int y, unsigned int x, puyocolor data);
	int GetCount();
	void IncreaseCount(int n = 1);
	void DecreaseCount(int n = 1);

protected:
	//盤面状態
	puyocolor *data;
	//盤面の行数，列数
	unsigned int data_line;
	unsigned int data_column;
	int data_count;
	void Release();
};

class PuyoArrayActive : public PuyoArray
{
public:

	PuyoArrayActive();
	void SetRotateState(const int puyo_rotate);
	int  GetRotateState(void)const;
private:
	int puyo_state_rotate;

};
class PuyoArrayStack : public PuyoArray
{
public:
	PuyoArrayStack();
	int VanishPuyo();
	int VanishPuyoDfs(unsigned int y, unsigned int x, puyocolor const &prev_puyo_color, bool *puyo_temp_dfs);
	bool GetVanished();
	int GetChainNum();
private:
	bool isVanished ; 
	int  m_chain;
};

class PuyoControl
{
public:
	PuyoControl(PuyoArrayActive &puyo_active_, PuyoArrayStack &puyo_stack_);
	void GeneratePuyo();
	void PopFromQueue();
	void GetQueue(std::queue<puyocolor> &stack_queue);
	void LandedPuyo(int y, int x);
	bool isLandingPuyo();
	void MoveLeft();
	void MoveRight();
	void MoveDown();
	void MoveStackDown();
	void RotateCw();
	void RotateCcw();
	const int DELAY_FORCE_MOVEDOWN;
	
private:
    PuyoArrayActive &puyo_active;
    PuyoArrayStack &puyo_stack;
	std::queue<puyocolor> newPuyoQueue;
	int newPuyoLandingCount;
};


#endif
