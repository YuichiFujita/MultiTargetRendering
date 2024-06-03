//============================================================
//
//	レンダーステートヘッダー [renderState.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _RENDER_STATE_H_
#define _RENDER_STATE_H_

//************************************************************
//	クラス定義
//************************************************************
// レンダーステートクラス
class CRenderState
{
public:
	// αブレンド列挙
	enum EBlend
	{
		BLEND_NORMAL = 0,	// 通常合成
		BLEND_ADD,			// 加算合成
		BLEND_SUB,			// 減算合成
		BLEND_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CRenderState();

	// デストラクタ
	~CRenderState();

	// αブレンド情報
	struct SBlendAlpha
	{
	public:
		// コンストラクタ
		SBlendAlpha() {}
		SBlendAlpha(const D3DBLENDOP Op, const D3DBLEND Scr, const D3DBLEND Dest)
		{ op = Op; scr = Scr; dest = Dest; bBlend = true; }

		// デストラクタ
		~SBlendAlpha() {}

		// メンバ変数
		D3DBLENDOP	op;		// αブレンド情報
		D3DBLEND	scr;	// SCRブレンド
		D3DBLEND	dest;	// DESTブレンド
		bool		bBlend;	// αブレンド状況
	};

	// αテスト情報
	struct STestAlpha
	{
		D3DCMPFUNC	func;	// αテスト情報
		int			nRef;	// αテスト参照値
		bool		bTest;	// αテスト状況
	};

	// Zテスト情報
	struct STestZ
	{
		D3DCMPFUNC	func;		// Zテスト情報
		bool		bUpdate;	// Zバッファ更新状況
	};

	// レンダーステート情報
	struct SInfo
	{
		SBlendAlpha	blendAlpha;	// αブレンド情報
		STestAlpha	testAlpha;	// αテスト情報
		STestZ		testZ;		// Zテスト情報
		D3DCULL		cull;		// カリング情報
		bool		bLight;		// ライティング状況
	};

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Set(void);		// 設定
	void Reset(void);	// 再設定
	void SetInfoRenderState(const SInfo& rInfo);	// レンダーステート情報設定
	SInfo GetInfoRenderState(void);					// レンダーステート情報取得
	void SetAlphaBlend(const EBlend blend);			// αブレンド情報設定

	void SetAlphaFunc(const D3DCMPFUNC func)	{ m_info.testAlpha.func		= func; }		// αテストの情報設定
	void SetAlphaNumRef(const int nRef)			{ m_info.testAlpha.nRef		= nRef; }		// αテストの参照値設定
	void SetAlphaTest(const bool bTest)			{ m_info.testAlpha.bTest	= bTest; }		// αテストの状況設定
	void SetZFunc(const D3DCMPFUNC func)		{ m_info.testZ.func			= func; }		// Zテストの情報設定
	void SetZUpdate(const bool bUpdate)			{ m_info.testZ.bUpdate		= bUpdate; }	// Zテストのバッファ更新設定

	void SetCulling(const D3DCULL cull)			{ m_info.cull	= cull; }	// カリング設定
	void SetLighting(const bool bLight)			{ m_info.bLight	= bLight; }	// ライティング設定

	// 静的メンバ関数
	static CRenderState *Create(void);	// 生成
	static void Release(CRenderState *&prRenderState);		// 破棄
	static void BindDevice(LPDIRECT3DDEVICE9 *ppDevice);	// デバイス割当
	static void InitRenderState(void);		// レンダーステート情報初期化
	static SInfo GetSaveRenderState(void);	// 保存レンダーステート情報取得

private:
	// 静的メンバ関数
	static void SetRenderState(const SInfo& rInfo);	// レンダーステート反映

	// 静的メンバ変数
	static LPDIRECT3DDEVICE9 *m_ppDevice;	// Direct3Dデバイスへのポインタ
	static SInfo m_save;	// 保存レンダーステート情報

	// メンバ変数
	SInfo m_info;	// レンダーステート情報
};

#endif	// _RENDER_STATE_H_
