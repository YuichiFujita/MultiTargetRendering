//============================================================
//
//	シネマスコープヘッダー [cinemaScope.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _CINEMA_SCOPE_H_
#define _CINEMA_SCOPE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"

//************************************************************
//	前方宣言
//************************************************************
class CObject2D;	// オブジェクト2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// シネマスコープクラス
class CCinemaScope
{
public:
	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,		// なにもしない
		STATE_SCOPE_IN,		// スコープイン
		STATE_SCOPE_OUT,	// スコープアウト
		STATE_MAX			// この列挙型の総数
	};

	// ポリゴン列挙
	enum EPolygon
	{
		POLYGON_TOP = 0,	// 上黒ポリゴン
		POLYGON_BOTTOM,		// 下黒ポリゴン
		POLYGON_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CCinemaScope();

	// デストラクタ
	~CCinemaScope();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(const float fDeltaTime);	// 更新
	void Draw(void);		// 描画
	void SetScopeIn(void);	// スコープイン設定
	void SetScopeOut(void);	// スコープアウト設定
	void SetEnableDraw(const bool bDraw);			// 描画状況設定
	EState GetState(void) const { return m_state; }	// 状態取得

	// 静的メンバ関数
	static CCinemaScope *Create(void);					// 生成
	static void Release(CCinemaScope *&prCinemaScope);	// 破棄

private:
	// メンバ関数
	void UpdateScopeIn(void);	// スコープインの更新
	void UpdateScopeOut(void);	// スコープアウトの更新

	// メンバ変数
	CObject2D *m_apBrack[POLYGON_MAX];	// 黒ポリゴンの情報
	EState m_state;			// 状態
	int m_nCounterScope;	// スコープ移動管理カウンター
};

#endif	// _CINEMA_SCOPE_H_
