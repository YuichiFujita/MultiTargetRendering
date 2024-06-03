//============================================================
//
//	チュートリアルマネージャーヘッダー [tutorialManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _TUTORIAL_MANAGER_H_
#define _TUTORIAL_MANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	前方宣言
//************************************************************
class CObject2D;	// オブジェクト2Dクラス
class CGauge2D;		// ゲージ2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// チュートリアルマネージャークラス
class CTutorialManager
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_CONTROL = 0,	// 操作方法表示のテクスチャ
		TEXTURE_MAX				// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,		// 何もしない状態
		STATE_NEXTWAIT,		// 次レッスン待機状態
		STATE_PROGRESSION,	// 進行状態
		STATE_END,			// 終了状態
		STATE_MAX			// この列挙型の総数
	};

	// レッスン列挙
	enum ELesson
	{
		LESSON_NONE = 0,	// レッスン無し
		LESSON_01,			// レッスン01：移動・カメラ
		LESSON_02,			// レッスン02：ジャンプ
		LESSON_03,			// レッスン03：回避
		LESSON_04,			// レッスン04：攻撃
		LESSON_05,			// レッスン05：終了
		LESSON_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CTutorialManager();

	// デストラクタ
	~CTutorialManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(const float fDeltaTime);	// 更新
	void AddLessonCounter(void);	// レッスンカウンター加算
	int GetLesson(void) const;		// レッスン取得
	EState GetState(void) const;	// 状態取得

	// 静的メンバ関数
	static CTutorialManager *Create(void);						// 生成
	static void Release(CTutorialManager *&prTutorialManager);	// 破棄
	static int GetNextLessonCounter(const int nID);				// レッスン移行カウント取得

private:
	// メンバ関数
	void SetEnableProgressionDraw(const bool bDraw);	// 進行状態時に表示するUIの描画状況設定
	void UpdateNextWait(void);	// 次レッスン待機
	void NextLesson(void);		// 次レッスンへの移行

	// メンバ変数
	CGauge2D *m_pCounterLesson;	// レッスン管理カウンターの情報
	CObject2D *m_pGuide;		// 小説明表示の情報
	CObject2D *m_pGuideBG;		// 小説明表示の背景情報
	CObject2D *m_pControl;		// 操作方法表示の情報
	CObject2D *m_pControlBG;	// 操作方法表示の背景情報
	EState m_state;				// 状態
	int m_nCounterState;		// 状態管理カウンター
	int m_nLesson;				// レッスン
};

#endif	// _TUTORIAL_MANAGER_H_
