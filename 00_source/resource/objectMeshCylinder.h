//============================================================
//
//	オブジェクトメッシュシリンダーヘッダー [objectMeshCylinder.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT_MESHCYLINDER_H_
#define _OBJECT_MESHCYLINDER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "renderState.h"

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトメッシュシリンダークラス
class CObjectMeshCylinder : public CObject
{
public:
	// テクスチャ方向列挙
	enum ETexDir
	{
		TEXDIR_OUTSIDE = 0,	// テクスチャ外側方向
		TEXDIR_INSIDE,		// テクスチャ内側方向
		TEXDIR_MAX			// この列挙型の総数
	};

	// コンストラクタ
	explicit CObjectMeshCylinder(const CObject::ELabel label = LABEL_NONE, const CObject::EDim dimension = DIM_3D, const int nPriority = object::DEFAULT_PRIO);

	// デストラクタ
	~CObjectMeshCylinder() override;

	// メッシュシリンダー構造体
	struct SMeshCylinder
	{
		D3DXVECTOR3	pos;		// 位置
		D3DXVECTOR3	rot;		// 向き
		D3DXCOLOR	col;		// 色
		D3DXMATRIX	mtxWorld;	// ワールドマトリックス
		ETexDir		texDir;		// テクスチャ方向
		float		fRadius;	// 半径
		float		fHeight;	// 縦幅
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;			// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	void SetVec3Rotation(const D3DXVECTOR3& rRot) override;	// 向き設定
	D3DXVECTOR3 GetVec3Position(void) const override	{ return m_meshCylinder.pos; }			// 位置取得
	D3DXVECTOR3 GetVec3Rotation(void) const override	{ return m_meshCylinder.rot; }			// 向き取得
	D3DXMATRIX *GetPtrMtxWorld(void) override			{ return &m_meshCylinder.mtxWorld; }	// マトリックスポインタ取得
	D3DXMATRIX GetMtxWorld(void) const override			{ return m_meshCylinder.mtxWorld; }		// マトリックス取得

	// 静的メンバ関数
	static CObjectMeshCylinder *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart,		// 分割数
		const POSGRID2& rTexPart,	// テクスチャ分割数
		const float fRadius,		// 半径
		const float fHeight			// 縦幅
	);

	// メンバ関数
	CRenderState *GetRenderState(void);				// レンダーステート情報取得
	void BindTexture(const int nTextureID);			// テクスチャ割当 (インデックス)
	void BindTexture(const char *pTexturePass);		// テクスチャ割当 (パス)
	void SetColor(const D3DXCOLOR& rCol);			// 色設定
	void SetRadius(const float fRadius);			// 半径設定
	void SetHeight(const float fHeight);			// 縦幅設定
	void SetTexDir(const ETexDir texDir);			// テクスチャ方向設定
	HRESULT SetPattern(const POSGRID2& rPart);		// 分割数設定
	void SetTexPattern(const POSGRID2& rTexPart);	// テクスチャ分割数設定
	int GetTextureIndex(void) const		{ return m_nTextureID; }			// テクスチャインデックス取得
	D3DXCOLOR GetColor(void) const		{ return m_meshCylinder.col; }		// 色取得
	float GetRadius(void) const			{ return m_meshCylinder.fRadius; }	// 半径取得
	float GetHeight(void) const			{ return m_meshCylinder.fHeight; }	// 縦幅取得
	ETexDir GetTexDir(void) const		{ return m_meshCylinder.texDir; }	// テクスチャ方向取得
	POSGRID2 GetPattern(void) const		{ return m_part; }					// 分割数取得
	POSGRID2 GetTexPattern(void) const	{ return m_texPart; }				// テクスチャ分割数取得

protected:
	// メンバ関数
	void SetVtx(void);	// 頂点情報の設定
	void SetIdx(void);	// インデックス情報の設定
	void SetScrollTex	// スクロールのテクスチャ座標の設定
	( // 引数
		const float fTexU,	// テクスチャの横座標の開始位置
		const float fTexV	// テクスチャの縦座標の開始位置
	);

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

	SMeshCylinder m_meshCylinder;	// メッシュシリンダーの情報
	POSGRID2 m_part;	// 分割数
	POSGRID2 m_texPart;	// テクスチャ分割数
	int m_nNumVtx;		// 必要頂点数
	int m_nNumIdx;		// 必要インデックス数
	int m_nTextureID;	// テクスチャインデックス
};

#endif	// _OBJECT_MESHCYLINDER_H_
