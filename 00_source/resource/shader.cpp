//============================================================
//
//	シェーダー処理 [shader.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "shader.h"
#include "shaderToon.h"
#include "shaderMono.h"
#include "manager.h"
#include "renderer.h"

//************************************************************
//	親クラス [CShader] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CShader::CShader() :
	m_pEffect			(nullptr),	// エフェクトポインタ
	m_pTechnique		(nullptr),	// テクニック関数
	m_pMtxWorld			(nullptr),	// ワールドマトリックス
	m_pMtxView			(nullptr),	// ビューマトリックス
	m_pMtxProjection	(nullptr)	// プロジェクションマトリックス
{

}

//============================================================
//	デストラクタ
//============================================================
CShader::~CShader()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CShader::Init(void)
{
	// メンバ変数を初期化
	m_pEffect			= nullptr;	// エフェクトポインタ
	m_pTechnique		= nullptr;	// テクニック関数
	m_pMtxWorld			= nullptr;	// ワールドマトリックス
	m_pMtxView			= nullptr;	// ビューマトリックス
	m_pMtxProjection	= nullptr;	// プロジェクションマトリックス

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CShader::Uninit(void)
{
	// エフェクトの破棄
	SAFE_RELEASE(m_pEffect);
}

//============================================================
//	ライト方向ベクトルの設定処理
//============================================================
void CShader::SetLightDirect(D3DXMATRIX * /*pMtxWorld*/, const int /*nLightID*/)
{

}

//============================================================
//	テクスチャの設定処理 (ポインタ)
//============================================================
void CShader::SetTexture(const CTexture::STexture& /*rTexture*/)
{

}

//============================================================
//	テクスチャの設定処理 (インデックス)
//============================================================
void CShader::SetTexture(const int /*nTextureID*/)
{

}

//============================================================
//	マテリアルの設定処理
//============================================================
void CShader::SetMaterial(const D3DMATERIAL9& /*rMaterial*/)
{

}

//============================================================
//	拡散光の設定処理
//============================================================
void CShader::SetDiffuse(const D3DXCOLOR& /*rDiffuse*/)
{

}

//============================================================
//	環境光の設定処理
//============================================================
void CShader::SetAmbient(const D3DXCOLOR& /*rAmbient*/)
{

}

//============================================================
//	放射光の設定処理
//============================================================
void CShader::SetEmissive(const D3DXCOLOR& /*rEmissive*/)
{

}

//============================================================
//	拡散光のみ設定処理
//============================================================
void CShader::SetOnlyDiffuse(const D3DXCOLOR& /*rDiffuse*/)
{

}

//============================================================
//	開始処理
//============================================================
void CShader::Begin(void)
{
	if (m_pEffect == nullptr) { assert(false); return; }	// エフェクト未使用

	// 開始
	m_pEffect->Begin(nullptr, 0);
}

//============================================================
//	パス開始処理
//============================================================
void CShader::BeginPass(const BYTE pass)
{
	if (m_pEffect == nullptr) { assert(false); return; }	// エフェクト未使用

	// パスの開始
	m_pEffect->BeginPass(pass);
}

//============================================================
//	パス終了処理
//============================================================
void CShader::EndPass(void)
{
	if (m_pEffect == nullptr) { assert(false); return; }	// エフェクト未使用

	// パスの終了
	m_pEffect->EndPass();
}

//============================================================
//	終了処理
//============================================================
void CShader::End(void)
{
	if (m_pEffect == nullptr) { assert(false); return; }	// エフェクト未使用

	// 終了
	m_pEffect->End();
}

//============================================================
//	マトリックスの設定処理
//============================================================
void CShader::SetMatrix(D3DXMATRIX* pMtxWorld)
{
	if (m_pEffect == nullptr) { assert(false); return; }	// エフェクト未使用

	// 変数を宣言
	D3DXMATRIX mtxView;			// ビューマトリックス
	D3DXMATRIX mtxProjection;	// プロジェクションマトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイス情報

	// ビューマトリックス・プロジェクションマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

	// 各マトリックスを設定
	m_pEffect->SetMatrix(m_pMtxWorld, pMtxWorld);
	m_pEffect->SetMatrix(m_pMtxView, &mtxView);
	m_pEffect->SetMatrix(m_pMtxProjection, &mtxProjection);
}

//============================================================
//	状態変更の伝達処理
//============================================================
void CShader::CommitChanges(void)
{
	if (m_pEffect == nullptr) { assert(false); return; }	// エフェクト未使用

	// 状態変更の伝達
	m_pEffect->CommitChanges();
}

//============================================================
//	エフェクト使用可能状況の取得処理
//============================================================
bool CShader::IsEffectOK(void) const
{
	// エフェクトポインタの使用状況を返す
	return (m_pEffect != nullptr) ? true : false;
}

//============================================================
//	エフェクトポインタ取得処理
//============================================================
LPD3DXEFFECT CShader::GetEffect(void) const
{
	// エフェクトポインタを返す
	return m_pEffect;
}

//============================================================
//	生成処理
//============================================================
HRESULT CShader::Create(void)
{
	// トゥーンシェーダーの生成
	CToonShader *pToonShader = CToonShader::Create();
	if (pToonShader == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 単色描画シェーダーの生成
	CMonoShader *pMonoShader = CMonoShader::Create();
	if (pMonoShader == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	破棄処理
//============================================================
void CShader::Release(void)
{
	// トゥーンシェーダーの破棄
	CToonShader::Release();

	// 単色描画シェーダーの破棄
	CMonoShader::Release();
}

//============================================================
//	エフェクトポインタの設定処理
//============================================================
void CShader::SetEffect(const LPD3DXEFFECT pEffect)
{
	// エフェクトポインタを設定
	m_pEffect = pEffect;
}

//============================================================
//	テクニック関数の設定処理
//============================================================
void CShader::SetTechnique(const D3DXHANDLE pTechnique)
{
	// テクニック関数を設定
	m_pTechnique = pTechnique;

	if (m_pEffect != nullptr)
	{ // エフェクトが使用されている場合

		// エフェクトにテクニック関数を設定
		m_pEffect->SetTechnique(m_pTechnique);
	}
	else { assert(false); }	// エフェクト使用不可
}
