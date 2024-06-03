//============================================================
//
//	チュートリアル画面ヘッダー [sceneTutorial.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SCENE_TUTORIAL_H_
#define _SCENE_TUTORIAL_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "scene.h"

//************************************************************
//	前方宣言
//************************************************************
class CTutorialManager;	// チュートリアルマネージャークラス

//************************************************************
//	クラス定義
//************************************************************
// チュートリアル画面クラス
class CSceneTutorial : public CScene
{
public:
	// コンストラクタ
	explicit CSceneTutorial(const EMode mode);

	// デストラクタ
	~CSceneTutorial() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新

	// 静的メンバ関数
	static CTutorialManager *GetTutorialManager(void);	// チュートリアルマネージャー取得

private:
	// 静的メンバ変数
	static CTutorialManager *m_pTutorialManager;	// チュートリアルマネージャー
};

#endif	// _SCENE_TUTORIAL_H_
