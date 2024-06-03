//============================================================
//
//	ローディングヘッダー [loading.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _LOADING_H_
#define _LOADING_H_

//************************************************************
//	前方宣言
//************************************************************
class CAnim2D;	// アニメーション2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// ローディングクラス
class CLoading
{
public:
	// ローディング状態列挙
	enum ELoading
	{
		LOAD_NONE = 0,	// 何もしない状態
		LOAD_FADEOUT,	// ロードの表示開始状態
		LOAD_UPDATE,	// ロード更新状態
		LOAD_WAIT,		// ロード待機状態
		LOAD_FADEIN,	// ロードの表示終了状態
		LOAD_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CLoading();

	// デストラクタ
	~CLoading();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(const float fDeltaTime);	// 更新
	void Draw(void);	// 描画
	HRESULT Set(std::function<HRESULT(bool*)> func);	// ロード開始設定
	ELoading GetState(void) const { return m_state; }	// ロード状態取得

	// 静的メンバ関数
	static CLoading *Create(void);				// 生成
	static void Release(CLoading *&pLoading);	// 破棄

private:
	// メンバ変数
	std::promise<HRESULT> *m_pPromise;	// プロミス
	std::future<HRESULT> *m_pFuture;	// フューチャー
	std::thread m_func;	// スレッド
	CAnim2D *m_pLoad;	// ロード画面情報
	ELoading m_state;	// ロード状態
	bool m_bEnd;		// ロード終了状況
};

#endif	// _LOADING_H_
