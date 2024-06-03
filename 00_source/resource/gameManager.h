//============================================================
//
//	ゲームマネージャーヘッダー [gameManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "retentionManager.h"

//************************************************************
//	クラス定義
//************************************************************
// ゲームマネージャークラス
class CGameManager
{
public:
	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,	// 何もしない状態
		STATE_NORMAL,	// 通常状態
		STATE_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CGameManager();

	// デストラクタ
	~CGameManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(const float fDeltaTime);	// 更新
	void SetState(const EState state);		// 状態設定
	EState GetState(void) const;			// 状態取得
	void TransitionResult(const CRetentionManager::EWin win);	// リザルト画面遷移

	// 静的メンバ関数
	static CGameManager *Create(void);	// 生成
	static void Release(CGameManager *&prGameManager);	// 破棄

private:
	// メンバ変数
	EState m_state;	// 状態
};

#endif	// _GAMEMANAGER_H_
