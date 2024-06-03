//============================================================
//
//	シェーダーヘッダー [shader.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SHADER_H_
#define _SHADER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "texture.h"

//************************************************************
//	クラス定義
//************************************************************
// シェーダークラス
class CShader
{
public:
	// コンストラクタ
	CShader();

	// デストラクタ
	virtual ~CShader();

	// 仮想関数
	virtual HRESULT Init(void);	// 初期化
	virtual void Uninit(void);	// 終了
	virtual void SetLightDirect(D3DXMATRIX *pMtxWorld, const int nLightID);	// ライト方向ベクトル設定
	virtual void SetTexture(const CTexture::STexture& rTexture);	// テクスチャ設定 (ポインタ)
	virtual void SetTexture(const int nTextureID);					// テクスチャ設定 (インデックス)
	virtual void SetMaterial(const D3DMATERIAL9& rMaterial);		// マテリアル設定
	virtual void SetDiffuse(const D3DXCOLOR& rDiffuse);				// 拡散光設定
	virtual void SetAmbient(const D3DXCOLOR& rAmbient);				// 環境光設定
	virtual void SetEmissive(const D3DXCOLOR& rEmissive);			// 放射光設定
	virtual void SetOnlyDiffuse(const D3DXCOLOR& rDiffuse);			// 拡散光のみ設定

	// メンバ関数
	void Begin(void);						// 開始
	void BeginPass(const BYTE pass);		// パス開始
	void EndPass(void);						// パス終了
	void End(void);							// 終了
	void SetMatrix(D3DXMATRIX *pMtxWorld);	// マトリックス設定
	void CommitChanges(void);				// 状態変更の伝達
	bool IsEffectOK(void) const;			// エフェクト使用可能状況の取得
	LPD3DXEFFECT GetEffect(void) const;		// エフェクトポインタ取得

	// 静的メンバ関数
	static HRESULT Create(void);	// 生成
	static void Release(void);		// 破棄

protected:
	// メンバ関数
	void SetEffect(const LPD3DXEFFECT pEffect);			// エフェクトポインタ設定
	void SetTechnique(const D3DXHANDLE pTechnique);		// テクニック関数設定
	void SetMatrixWorld(const D3DXHANDLE pMtxWorld)		{ m_pMtxWorld = pMtxWorld; }		// ワールドマトリックス設定
	void SetMatrixView(const D3DXHANDLE pMtxView)		{ m_pMtxView = pMtxView; }			// ビューマトリックス設定
	void SetMatrixProjection(const D3DXHANDLE pMtxProj)	{ m_pMtxProjection = pMtxProj; }	// プロジェクションマトリックス設定

private:
	// メンバ変数
	LPD3DXEFFECT m_pEffect;			// エフェクトポインタ
	D3DXHANDLE m_pTechnique;		// テクニック関数
	D3DXHANDLE m_pMtxWorld;			// ワールドマトリックス
	D3DXHANDLE m_pMtxView;			// ビューマトリックス
	D3DXHANDLE m_pMtxProjection;	// プロジェクションマトリックス
};

#endif	// _SHADER_H_
