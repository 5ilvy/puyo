#ifndef PUYO_GAME_CONTROL_H
#define PUYO_GAME_CONTROL_H

#include <curses.h>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <queue>

// (ぷよ型の中身)
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

// ===================================================
// データ基底クラス
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
    void AllClear();

protected:
    //盤面状態
    puyocolor *m_data;
    //盤面の行数，列数
    unsigned int m_data_line;
    unsigned int m_data_column;
    //ぷよの個数
    int m_data_count;
    void Release();
};

// ===================================================
// 落下中のぷよを管理するデータクラス
class PuyoArrayActive : public PuyoArray
{
public:
    PuyoArrayActive();
    void SetRotateState(const int puyo_rotate);
    int GetRotateState(void) const;

private:
    int m_puyoStateRotate;
};

// ===================================================
// 落下済みのぷよを管理するデータクラス
class PuyoArrayStack : public PuyoArray
{
public:
	PuyoArrayStack();
	int VanishPuyo();
	int VanishPuyoDfs(unsigned int y, unsigned int x, puyocolor const &prev_puyo_color, bool *puyo_temp_dfs);
	bool GetVanished();
	int GetChainNum();
	int GetCombo();
    int GetScore();
private:
	bool isVanished ; 
	int  m_score;
	int m_combo;
    int m_chain;
    int m_combomax;
};

class PuyoControl
{
public:
	PuyoControl();
	void GeneratePuyo();
	void PopFromQueue(PuyoArrayActive &puyo_active);
	void GetQueue(std::queue<puyocolor> &stack_queue);
	void LandedPuyo(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack, int y, int x);
	bool isLandingPuyo(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack);
	void MoveLeft(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack);
	void MoveRight(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack);
	void MoveDown(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack);
	void MoveStackDown(PuyoArrayStack &puyo_stack);
	void Rotate(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack);
	// void RotateCcw();
	const int DELAY_FORCE_MOVEDOWN;
	bool GameOverJudge(PuyoArrayStack &puyo_stack);
	
private:
	bool m_gameOverFlag;
	std::queue<puyocolor> m_newPuyoQueue;
	int m_newPuyoLandingCount;
   
};

#endif