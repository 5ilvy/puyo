#include "PGController.h"
#include "PuyoGameView.h"
#include "PuyoGameTools.h"


// クラス命名規則 19.06.08 update
// ====================================================
// 0. PGScene:基底を継承したクラスはPGS+(任意のシーン名)
// 1. メンバ変数は可能な限りキャメルケース+先頭にm_をつける
// 2. ポインタは 可能な限りpで始めるキャメルケースにする

// ====================================================
// シーン管理基底クラス
class PGScene{
public:
	//基底クラスになるので純粋仮想関数(=0をつけると明示できる)
    virtual void Start() = 0;  //初期化処理
    virtual void Update() = 0; //更新処理
};

// ====================================================
// スタート画面 < 基底:PuyoGameScene
class PGSStart:public PGScene{
public:
    void Start();	//初期化処理
    void Update();	//更新処理
};

// ====================================================
// プレイ画面 < 基底:PuyoGameScene
class PGSPlay:public PGScene{
public:
    PGSPlay();
    void Start();	//初期化処理
    void Update();	//更新処理
private:
    PuyoArrayActive m_PuyoActive;
	PuyoArrayStack  m_PuyoStack;
	PuyoControl     m_Control;
	PuyoView        m_View;
};

// ====================================================
// リザルト画面 < 基底:PuyoGameScene
class PGSResult:public PGScene{
public:
    void Start();	//初期化処理
    void Update();	//更新処理
};

// ====================================================
// 設定画面 < 基底:PuyoGameScene
class PGSConfig:public PGScene{
public:
    void Start();	//初期化処理
    void Update();	//更新処理
};

// ====================================================
// 終了画面 < 基底:PuyoGameScene
class PGSExit:public PGScene{
public:
    void Start(){};	//初期化処理
    void Update(){};	//更新処理
};


// ====================================================
// シーン管理 (Unityなどの猿真似)
class PGSceneManager{
public:
	//列挙型でシーンを管理
	enum GAMESCENE{
		TITLE,
		PLAY,
		RESULT,
		CONFIG,
        EXIT
	};
	//ゲーム自体は1個のインスタンスでいいのでstaticにすべき（？）
    static void newGame();                       // 新規ゲーム
	static void ChangeScene(GAMESCENE scene);    // シーン移行を担当
    static void Start();                    	 // イニシャライズ・1度しか実行しない処理
    static void Update();                     	 // ループの代わり
    static bool GetExitFlag();                   // 終了フラグを取得して返す
    static void UpdateScore(unsigned int score); // スコア更新用関数
    static unsigned int GetScore();              // スコア取得用関数
private:
	static PGScene* m_pScene;
    static GAMESCENE m_currentScene;
    static unsigned int m_score;
};

// ----------------------------------------------------
// シーン変更
void PGSceneManager::ChangeScene(GAMESCENE scene){
    // nullptr cpp03じゃ使えねえじゃねえか怒
    //     -> 自前で実装したので03でも動きますチクショー
    if (m_pScene != nullptr) delete m_pScene;
    // 分岐
    switch (scene)
    {
    case GAMESCENE::TITLE:
         m_pScene = new PGSStart();
        break;
    case GAMESCENE::PLAY:
         m_pScene = new PGSPlay();
        break;
    case GAMESCENE::RESULT:
         m_pScene = new PGSResult();
        break;
    case GAMESCENE::CONFIG:
         m_pScene = new PGSConfig();
        break;
    case GAMESCENE::EXIT:
         m_pScene = new PGSExit();
        break;
    default:
        break;
    }
    m_currentScene = scene;

}

// ----------------------------------------------------
// 今のシーンの初期化処理
void PGSceneManager::Start(){
    m_pScene->Start();
}

// ----------------------------------------------------
// 今のシーンの更新処理
void PGSceneManager::Update(){
    m_pScene->Update();
}
// ----------------------------------------------------
// 新規ゲーム開始
void PGSceneManager::newGame(){
    ChangeScene(GAMESCENE::TITLE);
    Start();
}
// ----------------------------------------------------
// 終了フラグを返す
bool PGSceneManager::GetExitFlag(){
    return (m_currentScene == GAMESCENE::EXIT);
}
// ----------------------------------------------------
// Updating Score
void PGSceneManager::UpdateScore(unsigned int score){
    m_score = score;
}
// ----------------------------------------------------
// Get Score
unsigned int PGSceneManager::GetScore(){
    return m_score;
}

// ----------------------------------------------------
// 
void PGSPlay::Start(){
    
}




// ========================================================

// static指定なので
PGScene *PGSceneManager::m_pScene=NULL;   

int main(int argc, char **argv)
{
    PGSceneManager puyo_game;
    puyo_game.newGame();
    while(!puyo_game.GetExitFlag()){
        puyo_game.Update();
    }
    return 0;
}