//============================================================
//
//	ランキングマネージャーヘッダー [rankingManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _RANKINGMANAGER_H_
#define _RANKINGMANAGER_H_

//************************************************************
//	定数宣言
//************************************************************
namespace ranking
{
	const int NUM_DISP = 5;	// ランキングの上位表示数
}

//************************************************************
//	前方宣言
//************************************************************
class CObject2D;	// オブジェクト2Dクラス
class CAnim2D;		// アニメーション2Dクラス
class CTimeUI;		// タイムUIクラス

//************************************************************
//	クラス定義
//************************************************************
// ランキングマネージャークラス
class CRankingManager
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_LOGO = 0,	// ランキングロゴテクスチャ
		TEXTURE_RANK,		// ランキング順位テクスチャ
		TEXTURE_MAX			// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,		// 何もしない状態
		STATE_FADEIN,		// フェードイン状態
		STATE_LOGO,			// ランキングロゴ表示状態
		STATE_RANK,			// 順位表示状態
		STATE_TIME_WAIT,	// クリアタイム表示待機状態
		STATE_TIME,			// クリアタイム表示状態
		STATE_WAIT,			// 遷移待機状態
		STATE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CRankingManager();

	// デストラクタ
	~CRankingManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(const float fDeltaTime);	// 更新

	// 静的メンバ関数
	static void Set(const float fValue);	// 設定
	static CRankingManager *Create(void);	// 生成
	static void Release(CRankingManager *&prRankingManager);	// 破棄

private:
	// メンバ関数
	void UpdateFade(void);		// フェードインの更新
	void RevisionFade(void);	// フェードインの補正
	void InitLogo(void);		// ランキングロゴ表示の初期化
	void UpdateLogo(void);		// ランキングロゴ表示の更新
	void RevisionLogo(void);	// ランキングロゴ表示の補正
	void InitRank(void);		// 順位表示の初期化
	void UpdateRank(void);		// 順位表示の更新
	void RevisionRank(void);	// 順位表示の補正
	void InitTime(void);		// クリアタイム表示の初期化
	void UpdateTime(void);		// クリアタイム表示の更新
	void RevisionTime(void);	// クリアタイム表示の補正
	void UpdateTransition(void);			// 遷移決定
	void SkipStaging(void);					// 演出スキップ
	bool UpdateDrawWait(const int nWait);	// 表示待機

	// 静的メンバ関数
	static void Sort(const float fValue);		// ソート
	static void SetNewRank(const float fValue);	// スコア変動インデックス設定
	static void Save(void);	// 保存
	static void Load(void);	// 読込

	// 静的メンバ変数
	static float m_aRanking[ranking::NUM_DISP];	// ランキング情報
	static int m_nNewRankID;	// 変動したスコアのインデックス

	// メンバ変数
	CTimeUI *m_apTime[ranking::NUM_DISP];	// クリアタイムの情報
	CAnim2D *m_apRank[ranking::NUM_DISP];	// 順位の情報
	CObject2D *m_pLogo;		// ランキングロゴの情報
	CObject2D *m_pFade;		// フェードの情報
	EState m_state;			// 状態
	int m_nCounterState;	// 状態管理カウンター
	int m_nCounterDraw;		// 描画管理カウンター
	float m_fScale;			// ポリゴン拡大率
};

#endif	// _RANKINGMANAGER_H_
