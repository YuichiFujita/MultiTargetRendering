//============================================================
//
//	オブジェクトメッシュキューブヘッダー [objectMeshCube.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT_MESHCUBE_H_
#define _OBJECT_MESHCUBE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "renderState.h"

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトメッシュキューブクラス
class CObjectMeshCube : public CObject
{
public:
	// コンストラクタ
	explicit CObjectMeshCube(const CObject::ELabel label = LABEL_NONE, const CObject::EDim dimension = DIM_3D, const int nPriority = object::DEFAULT_PRIO);

	// デストラクタ
	~CObjectMeshCube() override;

	// 原点列挙
	enum EOrigin
	{
		ORIGIN_CENTER = 0,	// 中心原点
		ORIGIN_DOWN,		// 下原点
		ORIGIN_UP,			// 上原点
		ORIGIN_MAX,			// この列挙型の総数
	};

	// 縁取り列挙
	enum EBorder
	{
		BORDER_OFF = 0,		// 縁取りなし
		BORDER_ON,			// 縁取りあり
		BORDER_MAX,			// この列挙型の総数
	};

	// テクスチャ使用列挙
	enum ETexState
	{
		TEXSTATE_ONE = 0,	// 同一テクスチャ
		TEXSTATE_SELECT,	// 全選択テクスチャ
		TEXSTATE_MAX,		// この列挙型の総数
	};

	// キューブ色列挙
	enum ECubeCol
	{
		CUBECOL_CUBE = 0,	// キューブ色
		CUBECOL_BORDER,		// 縁取り色
		CUBECOL_MAX			// この列挙型の総数
	};

	// キューブ分割数列挙
	enum ECubePart
	{
		CUBEPART_X = 0,		// テクスチャ分割数X
		CUBEPART_Y,			// テクスチャ分割数Y
		CUBEPART_Z,			// テクスチャ分割数Z
		CUBEPART_MAX		// この列挙型の総数
	};

	// テクスチャ構造体
	struct SFaceTex
	{
	public:
		// コンストラクタ
		SFaceTex() {}

		SFaceTex(const int nAll)
		{ All = nAll; Left = 0; Right = 0; Bottom = 0; Top = 0; Near = 0; Far = 0; }

		SFaceTex(const int nLeft, const int nRight, const int nBottom, const int nTop, const int nNear, const int nFar)
		{ All = 0; Left = nLeft; Right = nRight; Bottom = nBottom; Top = nTop; Near = nNear; Far = nFar; }

		SFaceTex(const int nAll, const int nLeft, const int nRight, const int nBottom, const int nTop, const int nNear, const int nFar)
		{ All = nAll; Left = nLeft; Right = nRight; Bottom = nBottom; Top = nTop; Near = nNear; Far = nFar; }

		// デストラクタ
		~SFaceTex() {}

		// メンバ変数
		int All;	// 全面のテクスチャインデックス
		int Left;	// 左のテクスチャインデックス
		int Right;	// 右のテクスチャインデックス
		int Bottom;	// 下のテクスチャインデックス
		int Top;	// 上のテクスチャインデックス
		int Near;	// 前のテクスチャインデックス
		int Far;	// 後のテクスチャインデックス
	};

	// メッシュキューブ構造体
	struct SMeshCube
	{
		D3DXVECTOR3	pos;					// 位置
		D3DXVECTOR3	rot;					// 向き
		D3DXVECTOR3	size;					// 大きさ
		D3DXCOLOR	aCol[CUBECOL_MAX];		// 色
		D3DXMATRIX	mtxWorld;				// ワールドマトリックス
		EBorder		bordState;				// 縁取り使用状態
		float		fBordThick;				// 縁取り太さ
		ETexState	texState;				// テクスチャ使用状態
		SFaceTex	texID;					// テクスチャ種類
		D3DXVECTOR2	aTexPart[CUBEPART_MAX];	// テクスチャ分割数
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;			// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	void SetVec3Rotation(const D3DXVECTOR3& rRot) override;	// 向き設定
	void SetVec3Sizing(const D3DXVECTOR3& rSize) override;	// 大きさ設定
	D3DXVECTOR3 GetVec3Position(void) const override	{ return m_meshCube.pos; }			// 位置取得
	D3DXVECTOR3 GetVec3Rotation(void) const override	{ return m_meshCube.rot; }			// 向き取得
	D3DXVECTOR3 GetVec3Sizing(void) const override		{ return m_meshCube.size; }			// 大きさ取得
	D3DXMATRIX *GetPtrMtxWorld(void) override			{ return &m_meshCube.mtxWorld; }	// マトリックスポインタ取得
	D3DXMATRIX GetMtxWorld(void) const override			{ return m_meshCube.mtxWorld; }		// マトリックス取得

	// 静的メンバ関数
	static CObjectMeshCube *Create		// 生成
	( // 引数
		const D3DXVECTOR3&	rPos,		// 位置
		const D3DXVECTOR3&	rRot,		// 向き
		const D3DXVECTOR3&	rSize,		// 大きさ
		const D3DXCOLOR&	rCubeCol,	// キューブ色
		const D3DXCOLOR&	rBorderCol	= XCOL_BLACK,			// 縁取り色
		const EBorder		bordState	= BORDER_OFF,			// 縁取り状態
		const float			fBordThick	= 0.0f,					// 縁取り太さ
		const ETexState		texState	= TEXSTATE_ONE,			// テクスチャ状態
		const SFaceTex&		rTexID		= SFaceTex(NONE_IDX),	// テクスチャ種類
		const EOrigin		origin		= ORIGIN_CENTER,		// 原点
		const D3DXVECTOR2&	rTexPartX	= VEC2_ONE,				// テクスチャ分割数X
		const D3DXVECTOR2&	rTexPartY	= VEC2_ONE,				// テクスチャ分割数Y
		const D3DXVECTOR2&	rTexPartZ	= VEC2_ONE				// テクスチャ分割数Z
	);

	// メンバ関数
	CRenderState *GetRenderState(void);						// レンダーステート情報取得
	void BindTexture(const SFaceTex textureID);				// テクスチャ割当
	void SetCubeColor(const D3DXCOLOR& rCol);				// キューブ色設定
	void SetBorderColor(const D3DXCOLOR& rCol);				// 縁取り色設定
	HRESULT SetBorderState(const EBorder bordState);		// 縁取り状態設定
	void SetBorderThick(const float fBordThick);			// 縁取り太さ設定
	void SetTextureState(const ETexState texState);			// テクスチャ状態設定
	void SetTexturePatternX(const D3DXVECTOR2& rTexPart);	// テクスチャ分割数X設定
	void SetTexturePatternY(const D3DXVECTOR2& rTexPart);	// テクスチャ分割数Y設定
	void SetTexturePatternZ(const D3DXVECTOR2& rTexPart);	// テクスチャ分割数Z設定
	void SetOrigin(const EOrigin origin);					// 原点設定
	SFaceTex GetTextureIndex(void) const		{ return m_meshCube.texID; }				// テクスチャインデックス取得
	D3DXCOLOR GetCubeColor(void) const			{ return m_meshCube.aCol[CUBECOL_CUBE]; }	// キューブ色取得
	D3DXCOLOR GetBorderColor(void) const		{ return m_meshCube.aCol[CUBECOL_BORDER]; }	// 縁取り色取得
	EBorder GetBorderState(void) const			{ return m_meshCube.bordState; }			// 縁取り状態取得
	float GetBorderThick(void) const			{ return m_meshCube.fBordThick; }			// 縁取り太さ取得
	ETexState GetTextureState(void) const		{ return m_meshCube.texState; }				// テクスチャ状態取得
	D3DXVECTOR2 GetTexturePatternX(void) const	{ return m_meshCube.aTexPart[CUBEPART_X]; }	// テクスチャ分割数X取得
	D3DXVECTOR2 GetTexturePatternY(void) const	{ return m_meshCube.aTexPart[CUBEPART_Y]; }	// テクスチャ分割数Y取得
	D3DXVECTOR2 GetTexturePatternZ(void) const	{ return m_meshCube.aTexPart[CUBEPART_Z]; }	// テクスチャ分割数Z取得
	EOrigin GetOrigin(void) const				{ return m_origin; }						// 原点取得

protected:
	// メンバ関数
	void SetVtx(void);	// 頂点情報の設定
	void SetIdx(void);	// インデックス情報の設定

private:
	// オーバーライド関数
	void Release(void) override { CObject::Release(); }	// 破棄

	// メンバ関数
	void DrawNormal(void);	// 通常描画
	void DrawShader(CShader *pShader);	// シェーダー描画

	// メンバ変数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;	// インデックスバッファへのポインタ
	CRenderState *m_pRenderState;		// レンダーステートの情報

	SMeshCube m_meshCube;	// メッシュキューブの情報
	EOrigin m_origin;		// 原点
	int m_nNumVtx;			// 必要頂点数
	int m_nNumIdx;			// 必要インデックス数
};

#endif	// _OBJECT_MESHCUBE_H_
