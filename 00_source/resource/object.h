//============================================================
//
//	オブジェクトヘッダー [object.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT_H_
#define _OBJECT_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "shader.h"
#include "shaderToon.h"
#include "shaderMono.h"

//************************************************************
//	定数宣言
//************************************************************
namespace object
{
	const DWORD FVF_VERTEX_2D		= (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);				// 頂点フォーマット [2D]
	const DWORD FVF_VERTEX_3D		= (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);	// 頂点フォーマット [3D]
	const DWORD FVF_VERTEX_MULTEX3D	= (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2);	// 頂点フォーマット [MULTEX3D]

	const int MAX_PRIO		= 8;	// 優先順位の総数
	const int DEFAULT_PRIO	= 3;	// デフォルトの優先順位
}

//************************************************************
//	構造体定義
//************************************************************
// 頂点情報 [2D]
struct VERTEX_2D
{
	D3DXVECTOR3	pos;	// 頂点座標
	float		rhw;	// 座標変換用係数 (1.0fで固定)
	D3DCOLOR	col;	// 頂点カラー
	D3DXVECTOR2	tex;	// テクスチャ座標
};

// 頂点情報 [3D]
struct VERTEX_3D
{
	D3DXVECTOR3	pos;	// 頂点座標
	D3DXVECTOR3	nor;	// 法線ベクトル
	D3DCOLOR	col;	// 頂点カラー
	D3DXVECTOR2	tex;	// テクスチャ座標
};

// 頂点情報 [MULTEX3D]
struct VERTEX_MULTEX3D
{
	D3DXVECTOR3	pos;	// 頂点座標
	D3DXVECTOR3	nor;	// 法線ベクトル
	D3DCOLOR	col;	// 頂点カラー
	D3DXVECTOR2	tex0;	// テクスチャ座標_0枚目
	D3DXVECTOR2	tex1;	// テクスチャ座標_1枚目
};

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトクラス
class CObject
{
public:
	// 次元列挙
	enum EDim
	{
		DIM_3D = 0,	// 3D描画
		DIM_2D,		// 2D描画
		DIM_MAX		// この列挙型の総数
	};

	// ラベル列挙
	enum ELabel
	{
		LABEL_NONE = 0,		// なし
		LABEL_UI,			// UI
		LABEL_TIMER,		// タイマー
		LABEL_WAVE,			// 波動
		LABEL_FIRE,			// 炎
		LABEL_THUNDER,		// 雷
		LABEL_BLUR,			// ブラー
		LABEL_EFFECT,		// エフェクト
		LABEL_PARTICLE,		// パーティクル
		LABEL_PLAYER,		// プレイヤー
		LABEL_ENEMY,		// 敵
		LABEL_MAGIC_CIRCLE,	// 魔法陣
		LABEL_MAP,			// マップ
		LABEL_FIELD,		// 地面
		LABEL_WALL,			// 壁
		LABEL_SCENERY,		// 景色
		LABEL_SKY,			// 空
		LABEL_LIQUID,		// 液体
		LABEL_DEBUG,		// デバッグ
		LABEL_MAX			// この列挙型の総数
	};

	// コンストラクタ
	explicit CObject(const ELabel label = LABEL_NONE, const EDim dimension = DIM_3D, const int nPriority = object::DEFAULT_PRIO);

	// デストラクタ
	virtual ~CObject();

	// 純粋仮想関数
	virtual HRESULT Init(void)	= 0;	// 初期化
	virtual void Uninit(void)	= 0;	// 終了
	virtual void Release(void)	= 0;	// 破棄
	virtual void Update(const float fDeltaTime)		= 0;	// 更新
	virtual void Draw(CShader *pShader = nullptr)	= 0;	// 描画

	// 仮想関数
	virtual void SetLabel(const ELabel label);				// ラベル設定
	virtual void SetDimension(const EDim dimension);		// 次元設定
	virtual void SetPriority(const int nPriority);			// 優先順位設定
	virtual void SetEnableUpdate(const bool bUpdate);		// 更新状況設定
	virtual void SetEnableDraw(const bool bDraw);			// 描画状況設定
	virtual void SetVec2Position(const D3DXVECTOR2& rPos);	// 二軸の位置設定
	virtual void SetVec3Position(const D3DXVECTOR3& rPos);	// 三軸の位置設定
	virtual void SetVec3Rotation(const D3DXVECTOR3& rRot);	// 向き設定
	virtual void SetVec2Sizing(const D3DXVECTOR2& rSize);	// 二軸の大きさ設定
	virtual void SetVec3Sizing(const D3DXVECTOR3& rSize);	// 三軸の大きさ設定
	virtual void SetVec3Scaling(const D3DXVECTOR3& rSacle);	// 拡大率設定
	virtual D3DXVECTOR2 GetVec2Position(void) const;		// 二軸の位置取得
	virtual D3DXVECTOR3 GetVec3Position(void) const;		// 三軸の位置取得
	virtual D3DXVECTOR3 GetVec3Rotation(void) const;		// 向き取得
	virtual D3DXVECTOR2 GetVec2Sizing(void) const;			// 二軸の大きさ取得
	virtual D3DXVECTOR3 GetVec3Sizing(void) const;			// 三軸の大きさ取得
	virtual D3DXVECTOR3 GetVec3Scaling(void) const;			// 拡大率取得
	virtual D3DXMATRIX *GetPtrMtxWorld(void);				// マトリックスポインタ取得
	virtual D3DXMATRIX GetMtxWorld(void) const;				// マトリックス取得

	// 静的メンバ関数
	static void	ReleaseAll(const std::vector<ELabel> label);	// 全破棄 (ラベル指定)
	static void	ReleaseAll(void);						// 全破棄
	static void	UpdateAll(const float fDeltaTime);		// 全更新
	static void	DrawAll(void);							// 全描画
	static int	GetNumAll(void) { return m_nNumAll; }	// 総数取得
	static CObject *GetTop(const EDim dimension, const int nPriority) { return m_apTop[dimension][nPriority]; }	// 先頭オブジェクト取得
	static CObject *GetCur(const EDim dimension, const int nPriority) { return m_apCur[dimension][nPriority]; }	// 最後尾オブジェクト取得

	// メンバ関数
	ELabel	GetLabel(void) const		{ return m_label; }		// ラベル取得
	EDim	GetDimension(void) const	{ return m_dimension; }	// 次元取得
	int		GetPriority(void) const		{ return m_nPriority; }	// 優先順位取得
	DWORD	GetUniqueID(void) const		{ return m_dwID; }		// ユニークID取得
	bool	IsUpdate(void) const		{ return m_bUpdate; }	// 更新状況取得
	bool	IsDraw(void) const			{ return m_bDraw; }		// 描画状況取得
	bool	IsDeath(void) const			{ return m_bDeath; }	// 死亡フラグ取得
	CObject	*GetObject(void)			{ return this; }		// オブジェクト取得
	CObject	*GetPrev(void) const		{ return m_pPrev; }		// 前オブジェクト取得
	CObject	*GetNext(void) const		{ return m_pNext; }		// 次オブジェクト取得

#ifdef _DEBUG

	// 静的メンバ関数
	static void	SetEnableDebugDispAll(const bool bDisp2D, const bool bDisp3D);	// 全表示状況設定

	// メンバ関数
	void SetEnableDebugDisp(const bool bDisp)	{ m_bDebugDisp = bDisp; }	// 表示状況設定
	bool IsDebugDisp(void) const				{ return m_bDebugDisp; }	// 表示状況取得

#endif	// _DEBUG

private:
	// 静的メンバ関数
	static void DeathAll(void);	// 全死亡

	// 静的メンバ変数
	static CObject	*m_apTop[DIM_MAX][object::MAX_PRIO];	// 先頭のオブジェクトへのポインタ
	static CObject	*m_apCur[DIM_MAX][object::MAX_PRIO];	// 最後尾のオブジェクトへのポインタ
	static DWORD	m_dwNextID;	// 次のユニークID
	static int		m_nNumAll;	// オブジェクトの総数

	// メンバ変数
	ELabel	m_label;		// 自身のオブジェクトラベル
	EDim	m_dimension;	// 自身の次元
	int		m_nPriority;	// 自身の優先順位
	DWORD	m_dwID;			// 自身のユニークID
	bool	m_bUpdate;		// 自身の更新状況
	bool	m_bDraw;		// 自身の描画状況
	bool	m_bDeath;		// 自身の死亡フラグ
	CObject	*m_pPrev;		// 前のオブジェクトへのポインタ
	CObject	*m_pNext;		// 次のオブジェクトへのポインタ

#ifdef _DEBUG

	bool m_bDebugDisp;	// 自身の表示状況

#endif	// _DEBUG

};

#endif	// _OBJECT_H_
