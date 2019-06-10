#include "PGController.h"

// ============================================================
// PuyoArray  | Puyoの盤面データの基底クラス
// PuyoArray::コンストラクタ
PuyoArray::PuyoArray()
    : m_data(NULL), m_data_line(0), m_data_column(0), m_data_count(0)
{
    //乱数初期化
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 5; i++)
        rand(); //カラ実行
}

// ============================================================
// PuyoArray  | Puyoの盤面データの基底クラス
// PuyoArray::デストラクタ
PuyoArray::~PuyoArray() { 
    Release();
    }

// ============================================================
// PuyoArray::メモリ開放
void PuyoArray::Release()
{
    if (m_data == NULL)
        return;
    delete[] m_data;
    m_data = NULL;
}

// ============================================================
// PuyoArray::盤面サイズ変更
void PuyoArray::ChangeSize(unsigned int line, unsigned int column)
{
    Release();

    //新しいサイズでメモリ確保
    m_data = new puyocolor[line * column];

    m_data_line = line;
    m_data_column = column;
}

// ============================================================
// PuyoArray::盤面の行数を返す
unsigned int PuyoArray::GetLine()
{
    return m_data_line;
}

// ============================================================
// PuyoArray 盤面をクリアする
void PuyoArray::AllClear(){
    for (int y = 0; y<GetLine(); y++)
     for (int x = 0; x<GetColumn(); x++)
        m_data [y*GetColumn()+x] = NONE;
}

// ============================================================
// PuyoArray::盤面の列数を返す
unsigned int PuyoArray::GetColumn()
{
    return m_data_column;
}

// ============================================================
// PuyoArray::盤面の指定された位置の値を返す
puyocolor PuyoArray::GetValue(unsigned int y, unsigned int x)
{
    if (y >= GetLine() || x >= GetColumn())
    {
        //引数の値が正しくない
        return NONE;
    }
    return m_data[y * GetColumn() + x];
}

// ============================================================
// PuyoArray::盤面の指定された位置に値を書き込む
void PuyoArray::SetValue(unsigned int y, unsigned int x, puyocolor value)
{
    if (y >= GetLine() || x >= GetColumn())
    {
        //引数の値が正しくない
        return;
    }

    m_data[y * GetColumn() + x] = value;
}

// ============================================================
// PuyoArray::盤面の中のぷよの個数を取得する関数(手動)
int PuyoArray::GetCount(void)
{
    return m_data_count;
}

// ============================================================
// PuyoArray::ぷよの個数カウント変数のインクリメント
void PuyoArray::IncreaseCount(int n)
{
    m_data_count += n;
}

// ============================================================
// PuyoArray::ぷよの個数カウント変数のデクリメント
void PuyoArray::DecreaseCount(int n)
{
    m_data_count -= n;
    if (m_data_count < 1)
        m_data_count = 0;
}

// -----------------------------------------------------------------------------------------------

// ============================================================
// PuyoArrayActive  | 落下中Puyoの盤面データ(基底:PuyoArray)
// PuyoArrayActive::コンストラクタ
PuyoArrayActive::PuyoArrayActive()
    : m_puyoStateRotate(0)
{
}

// ============================================================
// PuyoArrayActive::ぷよの回転状態の取得とセットをする関数群
void PuyoArrayActive::SetRotateState(const int puyo_rotate) { m_puyoStateRotate = puyo_rotate; }
int PuyoArrayActive::GetRotateState(void) const { return m_puyoStateRotate; }

// -----------------------------------------------------------------------------------------------

// ============================================================
// PuyoArrayStack  | 落下済Puyoの盤面データ(基底:PuyoArray)
// PuyoArrayStack::コンストラクタ
PuyoArrayStack::PuyoArrayStack()
    : isVanished(false), m_score(0), m_combo(0), m_combomax(0), m_chain(0)
{
}

// ============================================================
// PuyoArrayStack::ぷよのコンボ数の取得
int PuyoArrayStack::GetChainNum() { return m_score; }

// ============================================================
// PuyoArrayStack::ぷよが消えたかどうかを返す
bool PuyoArrayStack::GetVanished() { return isVanished; }

// ============================================================
// PuyoArrayStack::ぷよの連鎖を検出
int PuyoArrayStack::VanishPuyo()
{
    m_chain = 0;
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
                // m_score += ret * m_combo;
                for (int y_ = 0; y_ < GetLine(); y_++)
                {
                    for (int x_ = 0; x_ < GetColumn(); x_++)
                    {
                        if (puyo_temp_dfs[y_ * GetColumn() + x_])
                        {
                            // VanishAnimation();
                            puyo_temp_dfs[y_ * GetColumn() + x_] = false;
                            SetValue(y_, x_, NONE); //暫定
                        }
                    }
                }
                m_chain = ret;
                if (m_chain<0) m_chain = 0;
                exit_loop_flg = true;
            }
            if (exit_loop_flg)
                break;
                // usleep(10000);
        }
        if (exit_loop_flg)
            break;
    }
    delete[] puyo_temp_dfs;
    return m_chain;
}

// ============================================================
// PuyoArrayStack::盤面を深さ優先探索して同じ色のぷよをたどる
int PuyoArrayStack::VanishPuyoDfs(unsigned int y, unsigned int x, puyocolor const &prev_puyo_color, bool *puyo_temp_dfs)
{
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

// -----------------------------------------------------------------------------------------------

// ============================================================
// PuyoControl  | Puyoの操作など
// PuyoControl::コンストラクタ
PuyoControl::PuyoControl()
    : DELAY_FORCE_MOVEDOWN(1),
      m_newPuyoLandingCount(0),
      m_gameOverFlag(false)
{
}

// ============================================================
// PuyoControl::新しいぷよを生成してキューに格納
void PuyoControl::GeneratePuyo()
{
    puyocolor newpuyo;
    for (int i = 0; (i + 1) + m_newPuyoQueue.size() <= 10; i++)
    {
        newpuyo = (puyocolor)(rand() % 4 + 1);
        m_newPuyoQueue.push(newpuyo);
    }
}

// ============================================================
// PuyoControl::キューから新しいぷよを取り出し盤面に移す
void PuyoControl::PopFromQueue(PuyoArrayActive &puyo_active)
{
    for (int i = 0; i < 2; i++)
    {
        puyo_active.SetValue(0, 5 + i, m_newPuyoQueue.front());
        puyo_active.IncreaseCount();
        puyo_active.SetRotateState(0);
        m_newPuyoQueue.pop();
    }
    GeneratePuyo();
}

// ============================================================
// PuyoControl::キューの参照渡し
void PuyoControl::GetQueue(std::queue<puyocolor> &stack_queue)
{
    stack_queue = m_newPuyoQueue;
}

// ============================================================
// PuyoControl::ぷよの着地処理
void PuyoControl::LandedPuyo(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack, int y, int x)
{
    puyo_stack.SetValue(y, x, puyo_active.GetValue(y, x));
    puyo_stack.IncreaseCount();
    puyo_active.SetValue(y, x, NONE);
    puyo_active.DecreaseCount();
}

// ============================================================
// PuyoControl::ぷよの着地判定．着地判定があるとtrueを返す
bool PuyoControl::isLandingPuyo(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack)
{
    bool landed = false;
    static int new_puyo_count = 0;
    for (int y = puyo_active.GetLine() - 1; y >= 0; y--)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            if ((puyo_active.GetValue(y, x) != NONE) &&
                (puyo_stack.GetValue(y, x) == NONE) &&
                (puyo_stack.GetValue(y + 1, x) != NONE || y == puyo_active.GetLine() - 1))
            {
                LandedPuyo(puyo_active, puyo_stack, y, x);
                new_puyo_count++;
            }
        }
    }
    if (new_puyo_count == 2)
    {
        landed = true;
        new_puyo_count = 0;
    }
    m_newPuyoLandingCount = new_puyo_count;
    return landed;
}

// ============================================================
// PuyoControl::ぷよ左移動
void PuyoControl::MoveLeft(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack)
{
    if (m_newPuyoLandingCount == 1)
        return;
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
void PuyoControl::MoveRight(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack)
{
    if (m_newPuyoLandingCount == 1)
        return;
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
void PuyoControl::MoveDown(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack)
{
    for (int y = puyo_active.GetLine() - 1; y >= 0; y--)
    {
        for (int x = 0; x < puyo_active.GetColumn(); x++)
        {
            if (puyo_active.GetValue(y, x) == NONE)
            {
                continue;
            }
                if (y < puyo_active.GetLine() - 1
                     && puyo_stack.GetValue(y + 1, x) == NONE
                        && puyo_active.GetValue(y + 1, x) == NONE)
                {
                    puyo_active.SetValue(y + 1, x, puyo_active.GetValue(y, x));
                    //コピー後に元位置のpuyoactiveのデータは消す
                    puyo_active.SetValue(y, x, NONE);

            }
        }
    }
}

// ぷよを消したことで生じる穴を詰めてくれる関数
void PuyoControl::MoveStackDown(PuyoArrayStack &puyo_stack)
{
    bool isUnFinished = false;

    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[puyo_stack.GetLine() * puyo_stack.GetColumn()];

    do
    {
        isUnFinished = false;
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
                    isUnFinished = true;
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
    } while (isUnFinished);

    //一時的格納場所メモリ解放
    delete[] puyo_temp;
}

//回転(Clockwise)
void PuyoControl::Rotate(PuyoArrayActive &puyo_active, PuyoArrayStack &puyo_stack)
{
    if (m_newPuyoLandingCount == 1)
        return;
    puyocolor puyo_f, puyo_l;
    puyo_f = puyo_l = NONE;
    unsigned int puyo_f_pos[2] = {}, puyo_l_pos[2] = {};
    //0:y, 1:x

    for (int y = 0; y < puyo_active.GetLine() - 1; y++)
    {
        for (int x = 0; x < puyo_active.GetColumn() - 1; x++)
        {
            if (puyo_f != NONE && puyo_l != NONE)
                continue;
            if (puyo_active.GetValue(y, x) != NONE && puyo_f != NONE && puyo_l == NONE)
            {
                puyo_l = puyo_active.GetValue(y, x);
                puyo_l_pos[0] = y;
                puyo_l_pos[1] = x;
            }
            if (puyo_active.GetValue(y, x) != NONE && puyo_f == NONE)
            {
                puyo_f = puyo_active.GetValue(y, x);
                puyo_f_pos[0] = y;
                puyo_f_pos[1] = x;
            }
        }
    }
    puyo_active.SetValue(puyo_f_pos[0], puyo_f_pos[1], NONE);
    puyo_active.SetValue(puyo_l_pos[0], puyo_l_pos[1], NONE);
    bool rotateSuccess = false;
    switch (puyo_active.GetRotateState())
    {
    case 0: //AB -> |(A,B)
        if (puyo_l_pos[0] < puyo_active.GetLine() - 1 && puyo_stack.GetValue(puyo_f_pos[0] + 1, puyo_f_pos[1]) == NONE)
        {
            puyo_active.SetValue(puyo_f_pos[0], puyo_f_pos[1], puyo_f);
            puyo_active.SetValue(puyo_f_pos[0] + 1, puyo_f_pos[1], puyo_l);
            rotateSuccess = true;
        }
        break;
    case 1: //|(A,B) -> BA
        if (puyo_f_pos[1] > 0 && puyo_stack.GetValue(puyo_f_pos[0], puyo_f_pos[1] - 1) == NONE)
        {
            puyo_active.SetValue(puyo_f_pos[0], puyo_f_pos[1], puyo_f);
            puyo_active.SetValue(puyo_f_pos[0], puyo_f_pos[1] - 1, puyo_l);
            rotateSuccess = true;
        }
        break;
    case 2: //BA -> | (B,A) // A基準!!!!
        if (puyo_f_pos[0] > 0 && puyo_stack.GetValue(puyo_f_pos[0] - 1, puyo_l_pos[1]) == NONE)
        {
            puyo_active.SetValue(puyo_f_pos[0] - 1, puyo_l_pos[1], puyo_f);
            puyo_active.SetValue(puyo_l_pos[0], puyo_l_pos[1], puyo_l);
            rotateSuccess = true;
        }
        break;
    case 3: //|->AB
        if (puyo_l_pos[1] < puyo_active.GetColumn() - 1 && puyo_stack.GetValue(puyo_l_pos[0], puyo_l_pos[1] + 1) == NONE)
        {
            puyo_active.SetValue(puyo_l_pos[0], puyo_l_pos[1] + 1, puyo_f);
            puyo_active.SetValue(puyo_l_pos[0], puyo_l_pos[1], puyo_l);
            rotateSuccess = true;
        }
        break;
    }
    if (rotateSuccess)
    {
        puyo_active.SetRotateState((puyo_active.GetRotateState() + 1) % 4);
    }
    else
    {
        puyo_active.SetValue(puyo_f_pos[0], puyo_f_pos[1], puyo_f);
        puyo_active.SetValue(puyo_l_pos[0], puyo_l_pos[1], puyo_l);
    }
}

bool PuyoControl::GameOverJudge(PuyoArrayStack &puyo_stack)
{
    if ((puyo_stack.GetValue(0, 5) != NONE) || (puyo_stack.GetValue(0, 6) != NONE))
        m_gameOverFlag = true;
    return m_gameOverFlag;
}

int PuyoArrayStack::GetScore()
{
    return m_score;
}