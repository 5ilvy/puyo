#include "PuyoGameController.h"

PuyoArray::PuyoArray()
{
    data = NULL;
    data_line = 0;
    data_column = 0;
    data_count = 0;

    //乱数初期化
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 5; i++)
        rand(); //カラ実行
}

PuyoArray::~PuyoArray() { Release(); }

//メモリ開放
void PuyoArray::Release()
{
    if (data == NULL)
    {
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
    data = new puyocolor[line * column];

    data_line = line;
    data_column = column;
}

//盤面の行数を返す
unsigned int PuyoArray::GetLine()
{
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
    return data[y * GetColumn() + x];
}

//盤面の指定された位置に値を書き込む
void PuyoArray::SetValue(unsigned int y, unsigned int x, puyocolor value)
{
    if (y >= GetLine() || x >= GetColumn())
    {
        //引数の値が正しくない
        return;
    }

    data[y * GetColumn() + x] = value;
}

int PuyoArray::GetCount(void)
{
    return data_count;
}

void PuyoArray::IncreaseCount(int n)
{
    data_count += n;
}

void PuyoArray::DecreaseCount(int n)
{
    data_count -= n;
    if (data_count < 1)
        data_count = 0;
}

PuyoArrayStack::PuyoArrayStack()
{
    bool isVanished = false;
    int m_chain = 0;
}
int PuyoArrayStack::GetChainNum() { return m_chain; }
bool PuyoArrayStack::GetVanished() { return isVanished; }
// puyoの連鎖を検出
int PuyoArrayStack::VanishPuyo()
{
    bool *puyo_temp_dfs;
    puyo_temp_dfs = new bool[GetColumn() * GetLine()];
    bool exit_loop_flg = false;
    for (int y = 0; y < GetLine(); y++)
    {
        for (int x = 0; x < GetColumn(); x++)
        {
            puyocolor current_puyo = GetValue(y, x);
            if (current_puyo == NONE)
                continue;
            for (int i = 0; i < GetColumn() * GetLine(); i++)
                puyo_temp_dfs[i] = false;

            int ret = VanishPuyoDfs(y, x, current_puyo, puyo_temp_dfs);

            if (ret >= 4)
            {
                m_chain = ret;
                for (int y_ = 0; y_ < GetLine(); y_++)
                {
                    for (int x_ = 0; x_ < GetColumn(); x_++)
                    {
                        if (puyo_temp_dfs[y_ * GetColumn() + x_])
                            SetValue(y_, x_, NONE); //暫定
                    }
                }

                exit_loop_flg = true;
            }

            if (exit_loop_flg)
                break;
        }
        if (exit_loop_flg)
            break;
    }
    delete[] puyo_temp_dfs;

    return m_chain;
}

int PuyoArrayStack::VanishPuyoDfs(unsigned int y, unsigned int x, puyocolor const &prev_puyo_color, bool *puyo_temp_dfs)
{
    //validation(y, x)
    // if (x<0 || x>GetColumn()-1 ||y<0 || y>GetLine()-1)return false;
    if (GetValue(y, x) != prev_puyo_color || puyo_temp_dfs[y * GetColumn() + x] != false)
        return 0;
    int ret = 1;
    puyo_temp_dfs[y * GetColumn() + x] = true;
    if (y > 0)
        ret += VanishPuyoDfs(y - 1, x, prev_puyo_color, puyo_temp_dfs);
    if (y < GetLine() - 1)
        ret += VanishPuyoDfs(y + 1, x, prev_puyo_color, puyo_temp_dfs);
    if (x > 0)
        ret += VanishPuyoDfs(y, x - 1, prev_puyo_color, puyo_temp_dfs);
    if (x < GetColumn() - 1)
        ret += VanishPuyoDfs(y, x + 1, prev_puyo_color, puyo_temp_dfs);
    return ret;
}

PuyoControl::PuyoControl(PuyoArrayActive &puyo_active_, PuyoArrayStack &puyo_stack_) : puyo_active(puyo_active_), puyo_stack(puyo_stack_), DELAY_FORCE_MOVEDOWN(1)
{
}

//盤面に新しいぷよ生成
void PuyoControl::GeneratePuyo()
{
    puyocolor newpuyo;
    for (int i = 0; (i + 1) + newPuyoQueue.size() <= 10; i++)
    {
        newpuyo = (puyocolor)(rand() % 4 + 1);
        newPuyoQueue.push(newpuyo);
        
    }
}


void PuyoControl::PopFromQueue()
{

    for (int i = 0; i < 2; i++)
    {
        puyo_active.SetValue(0, 5 + i, newPuyoQueue.front());
        puyo_active.IncreaseCount();
        newPuyoQueue.pop();
    }
    GeneratePuyo();
}
void PuyoControl::GetQueue(std::queue<puyocolor> &stack_queue)
{
    stack_queue = newPuyoQueue;
}
void PuyoControl::LandedPuyo(int y, int x)
{

    puyo_stack.SetValue(y, x, puyo_active.GetValue(y, x));
    puyo_stack.IncreaseCount();
    puyo_active.SetValue(y, x, NONE);
    puyo_active.DecreaseCount();
}

//ぷよの着地判定．着地判定があるとtrueを返す
bool PuyoControl::isLandingPuyo()
{
    bool landed = false;
    static int new_puyo_count=0;
    for (int y = 0; y < puyo_active.GetLine(); y++)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            if ((puyo_active.GetValue(y, x) != NONE) &&
                (puyo_stack.GetValue(y, x) == NONE) &&
                (puyo_stack.GetValue(y + 1, x) != NONE || y == puyo_active.GetLine() - 1))
            {
                LandedPuyo(y, x);
                new_puyo_count++;
            }
        }
    }
    if (new_puyo_count == 2)
    {
        landed = true;
        new_puyo_count = 0;
    }
    return landed;
}

//左移動
void PuyoControl::MoveLeft()
{
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[puyo_active.GetLine() * puyo_active.GetColumn()];

    for (int i = 0; i < puyo_active.GetLine() * puyo_active.GetColumn(); i++)
    {
        puyo_temp[i] = NONE;
    }

    //1つ左の位置にpuyoactiveからpuyo_tempへとコピー
    for (int y = 0; y < puyo_active.GetLine(); y++)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            if (puyo_active.GetValue(y, x) == NONE)
            {
                continue;
            }

            if (0 < x && puyo_active.GetValue(y, x - 1) == NONE && puyo_stack.GetValue(y, x - 1) == NONE)
            {
                puyo_temp[y * puyo_active.GetColumn() + (x - 1)] = puyo_active.GetValue(y, x);
                //コピー後に元位置のpuyoactiveのデータは消す
                puyo_active.SetValue(y, x, NONE);
            }
            else
            {
                puyo_temp[y * puyo_active.GetColumn() + x] = puyo_active.GetValue(y, x);
            }
        }
    }

    //puyo_tempからpuyoactiveへコピー
    for (int y = 0; y < puyo_active.GetLine(); y++)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            puyo_active.SetValue(y, x, puyo_temp[y * puyo_active.GetColumn() + x]);
        }
    }

    //一時的格納場所メモリ解放
    delete[] puyo_temp;
}

//右移動
void PuyoControl::MoveRight()
{
    // if (new_puyo_count == 1)return;
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[puyo_active.GetLine() * puyo_active.GetColumn()];

    for (int i = 0; i < puyo_active.GetLine() * puyo_active.GetColumn(); i++)
    {
        puyo_temp[i] = NONE;
    }

    //1つ右の位置にpuyoactiveからpuyo_tempへとコピー
    for (int y = 0; y < puyo_active.GetLine(); y++)
    {
        for (int x = puyo_active.GetColumn() - 1; x >= 0; x--)
        {
            if (puyo_active.GetValue(y, x) == NONE)
                continue;
            //すりぬけちゃだめ！！ のための処理を追加
            if (x < puyo_active.GetColumn() - 1 &&
                puyo_active.GetValue(y, x + 1) == NONE &&
                puyo_stack.GetValue(y, x + 1) == NONE)
            {
                puyo_temp[y * puyo_active.GetColumn() + (x + 1)] = puyo_active.GetValue(y, x);
                //コピー後に元位置のpuyoactiveのデータは消す
                puyo_active.SetValue(y, x, NONE);
            }
            else
            {
                puyo_temp[y * puyo_active.GetColumn() + x] = puyo_active.GetValue(y, x);
            }
        }
    }

    //puyo_tempからpuyoactiveへコピー
    for (int y = 0; y < puyo_active.GetLine(); y++)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            puyo_active.SetValue(y, x, puyo_temp[y * puyo_active.GetColumn() + x]);
        }
    }

    //一時的格納場所メモリ解放
    delete[] puyo_temp;
}

//下移動
void PuyoControl::MoveDown()
{
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[puyo_active.GetLine() * puyo_active.GetColumn()];

    for (int i = 0; i < puyo_active.GetLine() * puyo_active.GetColumn(); i++)
    {
        puyo_temp[i] = NONE;
    }

    //1つ下の位置にpuyoactiveからpuyo_tempへとコピー
    for (int y = puyo_active.GetLine() - 1; y >= 0; y--)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            if (puyo_active.GetValue(y, x) == NONE)
            {
                continue;
            }

            if (y < puyo_active.GetLine() - 1 && puyo_stack.GetValue(y + 1, x) == NONE)
            {
                puyo_temp[(y + 1) * puyo_active.GetColumn() + x] = puyo_active.GetValue(y, x);
                //コピー後に元位置のpuyoactiveのデータは消す
                puyo_active.SetValue(y, x, NONE);
            }
            else
            {
                puyo_temp[y * puyo_active.GetColumn() + x] = puyo_active.GetValue(y, x);
            }
        }
    }

    //puyo_tempからpuyoactiveへコピー
    for (int y = 0; y < puyo_active.GetLine(); y++)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            puyo_active.SetValue(y, x, puyo_temp[y * puyo_active.GetColumn() + x]);
        }
    }

    //一時的格納場所メモリ解放
    delete[] puyo_temp;
}

// ぷよを消したことで生じる穴を詰めてくれる関数
void PuyoControl::MoveStackDown()
{
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[puyo_stack.GetLine() * puyo_stack.GetColumn()];

    for (int i = 0; i < puyo_stack.GetLine() * puyo_stack.GetColumn(); i++)
    {
        puyo_temp[i] = NONE;
    }

    //1つ下の位置にpuyoactiveからpuyo_tempへとコピー
    for (int y = puyo_stack.GetLine() - 1; y >= 0; y--)
    {
        for (int x = 0; x < puyo_stack.GetColumn(); x++)
        {
            if (puyo_stack.GetValue(y, x) == NONE)
            {
                continue;
            }

            if (y < puyo_stack.GetLine() - 1 && puyo_stack.GetValue(y + 1, x) == NONE)
            {
                puyo_temp[(y + 1) * puyo_stack.GetColumn() + x] = puyo_stack.GetValue(y, x);
                //コピー後に元位置のpuyoactiveのデータは消す
                puyo_stack.SetValue(y, x, NONE);
            }
            else
            {
                puyo_temp[y * puyo_stack.GetColumn() + x] = puyo_stack.GetValue(y, x);
            }
        }
    }

    //puyo_tempからpuyoactiveへコピー
    for (int y = 0; y < puyo_stack.GetLine(); y++)
    {
        for (int x = 0; x < puyo_stack.GetColumn(); x++)
        {
            puyo_stack.SetValue(y, x, puyo_temp[y * puyo_stack.GetColumn() + x]);
        }
    }

    //一時的格納場所メモリ解放
    delete[] puyo_temp;
}

//回転(Clockwise)
void PuyoControl::RotateCw()
{
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[puyo_active.GetLine() * puyo_active.GetColumn()];
    bool blkflg=false;
    for (int i = 0; i < puyo_active.GetLine() * puyo_active.GetColumn(); i++)
    {
        puyo_temp[i] = NONE;
    }
    for (int y = 0; y < puyo_active.GetLine() - 2; y++)
    {
        for (int x = 0; x < puyo_active.GetColumn() - 1; x++)
        {
            puyocolor now_puyo = puyo_active.GetValue(y, x);
            if (now_puyo == NONE)continue;
            puyocolor now_puyo_right = NONE;
            puyocolor now_puyo_lower = NONE;
            puyocolor now_puyo_left = NONE;
            puyocolor now_puyo_upper = NONE;

            if (now_puyo != NONE)
            {
                if (x < puyo_active.GetColumn() - 1 && puyo_stack.GetValue(y, x + 1) == NONE)
                    now_puyo_right = puyo_active.GetValue(y, x + 1);
                if (x > 0 && puyo_stack.GetValue(y, x - 1) == NONE)
                    now_puyo_left = puyo_active.GetValue(y, x - 1);
                if (y < puyo_active.GetLine() - 1)
                    now_puyo_lower = puyo_active.GetValue(y + 1, x);
                if (y > 0 )
                    now_puyo_upper = puyo_active.GetValue(y - 1, x);
                if (now_puyo_right != NONE)
                {
                    if (y<puyo_active.GetLine()-1 && puyo_stack.GetValue(y + 1, x) == NONE)
                    {
                        puyo_temp[y * puyo_active.GetColumn() + x] = now_puyo;
                        puyo_temp[(y + 1) * puyo_active.GetColumn() + x] = now_puyo_right;
                    }
                    else
                    {
                        puyo_temp[y * puyo_active.GetColumn() + x] = now_puyo;
                        puyo_temp[y * puyo_active.GetColumn() + x + 1] = now_puyo_right;
                    }
                }
                else if (now_puyo_left != NONE)
                {
                    if (y > 0 && puyo_stack.GetValue(y-1,x)==NONE)
                    {
                        puyo_temp[y * puyo_active.GetColumn() + x] = now_puyo;
                        puyo_temp[(y - 1) * puyo_active.GetColumn() + x] = now_puyo_left;
                    }
                    else
                    {
                        puyo_temp[y * puyo_active.GetColumn() + x] = now_puyo;
                        puyo_temp[y * puyo_active.GetColumn() + x - 1] = now_puyo_left;
                    }
                }
                else if (now_puyo_lower != NONE)
                {
                    if (x > 0 && puyo_stack.GetValue(y,x-1)==NONE)
                    {
                        puyo_temp[y * puyo_active.GetColumn() + x] = now_puyo;
                        puyo_temp[y * puyo_active.GetColumn() + x - 1] = now_puyo_lower;
                        blkflg = true;
                        break;
                    }
                    else
                    {
                        puyo_temp[y * puyo_active.GetColumn() + x] = now_puyo;
                        puyo_temp[(y + 1) * puyo_active.GetColumn() + x] = now_puyo_lower;
                    }
                }

                else if (now_puyo_upper != NONE)
                {
                    if (x < puyo_active.GetColumn() - 1 && puyo_stack.GetValue(y,x+1)==NONE)
                    {
                        puyo_temp[y * puyo_active.GetColumn() + x] = now_puyo;
                        puyo_temp[y * puyo_active.GetColumn() + x + 1] = now_puyo_upper;
                    }else{
                        puyo_temp[y * puyo_active.GetColumn() + x] = now_puyo;
                        puyo_temp[(y-1) * puyo_active.GetColumn() + x ] = now_puyo_upper;
                    }
                }
                blkflg = true;
                if (blkflg) break;
            }
            if (blkflg) break;
        }
        if (blkflg) break;
    }

    //puyo_tempからpuyoactiveへコピー
    for (int y = 0; y < puyo_active.GetLine(); y++)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            puyo_active.SetValue(y, x, puyo_temp[y * puyo_active.GetColumn() + x]);
        }
    }

    //一時的格納場所メモリ解放
    delete[] puyo_temp;
}
//回転(Counter-clockwise)
void PuyoControl::RotateCcw()
{
}