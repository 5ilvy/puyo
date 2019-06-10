#include "PGController.h"
#include "PuyoGameView.h"
#include "PuyoGameTools.h"

// クラス命名規則 19.06.08 update
// ====================================================
// 0. PGScene:基底を継承したクラスはPGS+(任意のシーン名)
// 1. メンバ変数は可能な限りキャメルケース+先頭にm_をつける
// 2. ポインタは 可能な限りpで始めるキャメルケースにする

//前方宣言
class PGScene;

// ====================================================
// シーン管理 (Unityなどの猿真似)
class PGSceneManager
{
public:
    //列挙型でシーンを管理
    enum GAMESCENE
    {
        TITLE,
        PLAY,
        RESULT,
        CONFIG,
        EXIT
    };
    //シーンの状態
    enum SCNSTATUS
    {
        WAITING,
        RUN,
        END
    };
    //ゲーム自体は1個のインスタンスでいいのでstaticにすべき（？）
    static void newGame();                       // 新規ゲーム
    static void ChangeScene(GAMESCENE scene);    // シーン移行を担当
    static void Start();                         // イニシャライズ・1度しか実行しない処理
    static void Update();                        // ループの代わり
    static bool GetExitFlag();                   // 終了フラグを取得して返す
    static void UpdateScore(unsigned int score); // スコア更新用関数
    static int GetScore();                       // スコア取得用関数
    static void Quit();                          // 終了処理

private:
    static PGScene *m_pScene;
    static GAMESCENE m_currentScene;
    static int m_score;
};

// ====================================================
// シーン管理基底クラス
class PGScene
{
public:
    //基底クラスになるので純粋仮想関数(=0をつけると明示できる)
    PGScene(PGSceneManager::GAMESCENE scene)
        : m_sceneStatus(PGSceneManager::RUN),
          m_nextScene(scene){
              PuyoView m_View;
          };
    virtual ~PGScene(){};
    virtual void Start() = 0;  //初期化処理
    virtual void Update() = 0; //更新処理
    PGSceneManager::SCNSTATUS m_sceneStatus;
    PGSceneManager::GAMESCENE m_nextScene;
    PuyoView m_View;
};

// ====================================================
// スタート・タイトル画面 < 基底:PuyoGameScene
class PGSTitle : public PGScene
{
public:
    PGSTitle() : PGScene(PGSceneManager::TITLE){
        
    };
    void Start();  //初期化処理
    void Update(); //更新処理
};

// ====================================================
// プレイ画面 < 基底:PuyoGameScene
class PGSPlay : public PGScene
{
public:
    PGSPlay() : PGScene(PGSceneManager::PLAY)
    {
        PuyoArrayActive m_PuyoActive;
        PuyoArrayStack m_PuyoStack;
        PuyoControl m_Control;

    };
    void Start();  //初期化処理
    void Update(); //更新処理
private:
    PuyoArrayActive m_PuyoActive;
    PuyoArrayStack m_PuyoStack;
    PuyoControl m_Control;

};

// ====================================================
// リザルト画面 < 基底:PuyoGameScene
class PGSResult : public PGScene
{
public:
    PGSResult() : PGScene(PGSceneManager::RESULT){};
    void Start();  //初期化処理
    void Update(); //更新処理
};

// ====================================================
// 設定画面 < 基底:PuyoGameScene
class PGSConfig : public PGScene
{
public:
    PGSConfig() : PGScene(PGSceneManager::CONFIG){};
    void Start();  //初期化処理
    void Update(); //更新処理
};

// ====================================================
// 終了画面 < 基底:PuyoGameScene
class PGSExit : public PGScene
{
public:
    PGSExit() : PGScene(PGSceneManager::EXIT){};
    void Start(){};  //初期化処理
    void Update(){}; //更新処理
};



// ----------------------------------------------------
// シーン変更
void PGSceneManager::ChangeScene(GAMESCENE scene)
{
    // nullPtr cpp03じゃ使えねえじゃねえか怒
    //     -> 自前で実装したので03でも動きますチクショー
    if (m_pScene != nullPtr)
        delete m_pScene;
    // シーン分岐
    switch (scene)
    {
    case TITLE:
        m_pScene = new PGSTitle();
        break;
    case PLAY:
        m_pScene = new PGSPlay();
        break;
    case RESULT:
        m_pScene = new PGSResult();
        break;
    // case CONFIG:
    //     m_pScene = new PGSConfig();
    //     break;
    case EXIT:
        m_pScene = new PGSExit();
        break;
    default:
        break;
    }
    m_currentScene = scene;
}

// ----------------------------------------------------
// 今のシーンの初期化処理
void PGSceneManager::Start()
{
    m_pScene->Start();
}

// ----------------------------------------------------
// 今のシーンの更新処理
void PGSceneManager::Update()
{
    m_pScene->Update();
    if (m_pScene->m_sceneStatus == END)
    {
        ChangeScene(m_pScene->m_nextScene);
        if (m_pScene->m_nextScene != EXIT)
            Start();
    }
}
// ----------------------------------------------------
// 新規ゲーム開始
void PGSceneManager::newGame()
{
    ChangeScene(TITLE);
    Start();
}
// ----------------------------------------------------
// 終了処理
void PGSceneManager::Quit()
{
    if (m_pScene != nullPtr)
        delete m_pScene;
    endwin();
}

// ----------------------------------------------------
// 終了フラグを返す
bool PGSceneManager::GetExitFlag()
{
    return (m_currentScene == EXIT);
}
// ----------------------------------------------------
// Updating Score
void PGSceneManager::UpdateScore(unsigned int score)
{
    m_score = score;
}
// ----------------------------------------------------
// Get Score
int PGSceneManager::GetScore()
{
    return m_score;
}

// ----------------------------------------------------
// プレイ画面:初期処理
void PGSPlay::Start()
{
    // 画面文字消去
    // ただし背景色が白か黒かは適当にきまる(仕様)
    for (int ay = 0; ay < LINES; ay++)
        for (int ax = 0; ax < COLS; ax++)
            mvaddch(ay, ax, ' ');

    m_PuyoActive.ChangeSize(LINES / 2, COLS / 2);
    m_PuyoStack.ChangeSize(LINES / 2, COLS / 2); //フィールドは画面サイズの縦横1/2にする
    m_PuyoActive.AllClear();
    m_PuyoStack.AllClear();
    m_Control.GeneratePuyo();                    //最初のぷよ生成
    m_Control.PopFromQueue(m_PuyoActive);        //ぷよ取り出し
}
// ----------------------------------------------------
// プレイ画面:更新処理
void PGSPlay::Update()
{
    refresh();
    // ゲームオーバーならgameSceneを書き換えてループを抜ける
    if (m_Control.GameOverJudge(m_PuyoStack))
    {
        m_sceneStatus = PGSceneManager::END;
        m_nextScene = PGSceneManager::RESULT;
        return;
    }
    //キー入力受付
    int ch_input = getch();

    //Qの入力で終了
    if (ch_input == 'q' || ch_input == 'Q')
    {
        m_sceneStatus = PGSceneManager::END;
        m_nextScene = PGSceneManager::EXIT;
        return;
    }
    // 入力キーごとの処理
    switch (ch_input)
    {
    // 左移動
    case KEY_LEFT:
        m_Control.MoveLeft(m_PuyoActive, m_PuyoStack);
        break;
    //右移動
    case KEY_RIGHT:
        m_Control.MoveRight(m_PuyoActive, m_PuyoStack);
        break;
    //即落下
    case ' ':
    {
        while (!m_Control.isLandingPuyo(m_PuyoActive, m_PuyoStack))
        {
            m_Control.MoveDown(m_PuyoActive, m_PuyoStack);
        }
        int numVanishedPuyo = 0;
        //ぷよ消去
        do
        {
            numVanishedPuyo = m_PuyoStack.VanishPuyo();
            m_Control.MoveStackDown(m_PuyoStack);
            // if 
        } while (numVanishedPuyo >= 4);
        //着地していたら新しいぷよ生成
        m_Control.PopFromQueue(m_PuyoActive);
        ch_input = 0;
    }
    break;
    //回転
    case 'z':
    {
        //ぷよ回転処理(着地判定されたら回転させず消滅させる)
        if (m_Control.isLandingPuyo(m_PuyoActive, m_PuyoStack))
        {
            int numVanishedPuyo = 0;
            //ぷよ消去
            do
            {
                numVanishedPuyo = m_PuyoStack.VanishPuyo();
                m_Control.MoveStackDown(m_PuyoStack);
                m_View.Display(m_PuyoActive, m_PuyoStack, m_Control);
                // m_View.ShowCombo()
                usleep(30000);
            } while (numVanishedPuyo >= 4);
            //着地していたら新しいぷよ生成
            m_Control.PopFromQueue(m_PuyoActive);
        }
        else
        {
            m_Control.Rotate(m_PuyoActive, m_PuyoStack);
        }
    }
    break;
    default:
        break;
    }

    //処理速度調整のためのif文
    if (m_View.delay % m_View.waitCount == 0)
    {
        //ぷよ下に移動
        m_Control.MoveDown(m_PuyoActive, m_PuyoStack);
        //ぷよ着地判定
        if (m_Control.isLandingPuyo(m_PuyoActive, m_PuyoStack))
        {
            int numVanishedPuyo = 0;
            //ぷよ消去
            do{
                numVanishedPuyo = m_PuyoStack.VanishPuyo();
                m_Control.MoveStackDown(m_PuyoStack);
            }while (numVanishedPuyo>=4);
            //着地していたら新しいぷよ生成
                m_Control.PopFromQueue(m_PuyoActive);
        }
    }
    m_Control.MoveStackDown(m_PuyoStack);
    //表示
    m_View.Display(m_PuyoActive, m_PuyoStack, m_Control);
}

// ----------------------------------------------------
// タイトル画面:初期処理
void PGSTitle::Start()
{
    //cursesの初期化など
    { //画面の初期化
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
    }

    m_View.GameTitleInit();
}

// ----------------------------------------------------
// タイトル画面:更新処理
void PGSTitle::Update()
{
    
    //キー入力受付
    int ch_input;
    ch_input = getch();
    //Qの入力で終了
    if (ch_input == 'q' || ch_input == 'Q')
    {
        m_sceneStatus = PGSceneManager::END;
        m_nextScene = PGSceneManager::EXIT;
        return;
    }
    //スペースキーが押されたらゲーム開始
    if (ch_input == ' ')
    {
        m_sceneStatus = PGSceneManager::END;
        m_nextScene = PGSceneManager::PLAY;
    }
    m_View.GameTitleAnimate();
}

// ----------------------------------------------------
// リザルト画面:初期処理
void PGSResult::Start(){
    initscr();
    m_View.GameOverModal(1000);
}

// ----------------------------------------------------
// リザルト画面:更新処理
void PGSResult::Update()
{
    
    static unsigned int current_item = 0;
    m_View.GameOverMenu(current_item);
    //キー入力受付
    int ch_input;
    ch_input = getch();
    switch (ch_input)
    {
    case KEY_UP:
        
        if (current_item == 0)
            current_item = 1;
            else
            {
                current_item = 0;
            }
            
        break;
    case KEY_DOWN:
        if (current_item == 1)
            current_item = 0;
            else
            {
                current_item = 1;
            }
            
        break;
    case ' ':
         m_View.GameOverMenuSelected(current_item);
        switch (current_item)
        {
        case 0:
            m_sceneStatus = PGSceneManager::END;
            m_nextScene = PGSceneManager::TITLE;
            break;
        case 1:
            m_sceneStatus = PGSceneManager::END;
            m_nextScene = PGSceneManager::EXIT;
            break;
        }
        
        break;
    }
    return;
}

// ========================================================
// ゲームの大元
// static指定なのではじめにヌルにしてあげないといけない
PGScene *PGSceneManager::m_pScene = nullPtr;
int PGSceneManager::m_score = 0;
PGSceneManager::GAMESCENE PGSceneManager::m_currentScene = TITLE;

int main(int argc, char **argv)
{
    PGSceneManager puyo_game;
    puyo_game.newGame();
    while (!puyo_game.GetExitFlag())
    {
        puyo_game.Update();
    }
    puyo_game.Quit();
    return 0;
}