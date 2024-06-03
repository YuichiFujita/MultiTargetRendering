//============================================================
//
//	トゥーンシェーダーヘッダー [shaderToon.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "shaderToon.h"
#include "manager.h"
#include "renderer.h"
#include "light.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャ定数
	{
		"data\\TEXTURE\\SHADER\\toon003.png",	// トゥーンマップテクスチャ
	};

	const char* EFFECT_FX = "data\\SHADER\\shaderToon.fx";	// トゥーンシェーダーのエフェクトファイル
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CToonShader::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CToonShader *CToonShader::m_pShader = nullptr;	// シェーダー情報

//************************************************************
//	親クラス [CToonShader] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CToonShader::CToonShader() :
	m_pTextureToon	(nullptr),	// トゥーンマップテクスチャ
	m_pDirectLight	(nullptr),	// 平行光源の方向ベクトル
	m_pDiffuse		(nullptr),	// 拡散光
	m_pAmbient		(nullptr),	// 環境光
	m_pEmissive		(nullptr),	// 放射光
	m_pUseTexture	(nullptr)	// テクスチャの使用状況
{

}

//============================================================
//	デストラクタ
//============================================================
CToonShader::~CToonShader()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CToonShader::Init(void)
{
	// 変数を宣言
	D3DCAPS9 caps;	// ハードウェア機能
	HRESULT hr;		// 異常終了の確認用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイス情報
	LPD3DXBUFFER pError  = nullptr;	// コンパイルエラー情報
	LPD3DXEFFECT pEffect = nullptr;	// エフェクト設定用

	// メンバ変数を初期化
	m_pTextureToon	= nullptr;	// トゥーンマップテクスチャ
	m_pDirectLight	= nullptr;	// 平行光源の方向ベクトル
	m_pDiffuse		= nullptr;	// 拡散光
	m_pAmbient		= nullptr;	// 環境光
	m_pEmissive		= nullptr;	// 放射光
	m_pUseTexture	= nullptr;	// テクスチャの使用状況

	// シェーダーの初期化
	if (FAILED(CShader::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ハードウェア機能を取得
	pDevice->GetDeviceCaps(&caps);

	if (caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)
	&&  caps.PixelShaderVersion  >= D3DPS_VERSION(2, 0))
	{ // 頂点・ピクセルシェーダのバージョンが使用可能な場合

		// エフェクトファイルの読込
		hr = D3DXCreateEffectFromFile
		( // 引数
			pDevice,	// デバイスへのポインタ
			EFFECT_FX,	// エフェクトファイル
			nullptr,	// プリプロセッサ定義
			nullptr,	// インクルード操作
			0,			// 読込オプションフラグ
			nullptr,	// グローバル変数インターフェース
			&pEffect,	// エフェクトインターフェース
			&pError		// コンパイルエラー情報
		);
		if (SUCCEEDED(hr))
		{ // 読込に成功した場合

			// エフェクト情報を設定
			SetEffect(pEffect);

			// テクニック関数を取得・設定
			SetTechnique(pEffect->GetTechniqueByName("TShader"));

			// 各マトリックスのグローバル変数を取得・設定
			SetMatrixWorld(pEffect->GetParameterByName(nullptr, "g_mtxWorld"));		// ワールドマトリックス
			SetMatrixView(pEffect->GetParameterByName(nullptr, "g_mtxView"));		// ビューマトリックス
			SetMatrixProjection(pEffect->GetParameterByName(nullptr, "g_mtxProj"));	// プロジェクションマトリックス

			// グローバル変数を取得
			m_pTextureToon	= pEffect->GetParameterByName(nullptr, "g_textureToon");	// トゥーンマップテクスチャ
			m_pDirectLight	= pEffect->GetParameterByName(nullptr, "g_dirLight");		// 平行光源の方向ベクトル
			m_pDiffuse		= pEffect->GetParameterByName(nullptr, "g_diffuse");		// 拡散光
			m_pAmbient		= pEffect->GetParameterByName(nullptr, "g_ambient");		// 環境光
			m_pEmissive		= pEffect->GetParameterByName(nullptr, "g_emissive");		// 放射光
			m_pUseTexture	= pEffect->GetParameterByName(nullptr, "g_bUseTex");		// テクスチャの使用状況

			// トゥーンマップテクスチャを設定
			SetToonMapTexture(TEXTURE_TOON);
		}
		else
		{ // 読込に失敗した場合

			// 読み込み失敗を返す
			assert(false);
			return E_FAIL;
		}
	}
	else
	{ // バージョンが使用不可な場合

		// 古いバージョンによる失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CToonShader::Uninit(void)
{
	// シェーダーの終了
	CShader::Uninit();
}

//============================================================
//	ライト方向ベクトルの設定処理
//============================================================
void CToonShader::SetLightDirect(D3DXMATRIX *pMtxWorld, const int nLightID)
{
	if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用

	// 変数を宣言
	D3DXVECTOR3 dirLight = GET_MANAGER->GetLight()->GetLight(nLightID).Direction;	// ライト方向計算用
	D3DXVECTOR4 setLight = D3DXVECTOR4(dirLight.x, dirLight.y, dirLight.z, 0.0f);	// ライト方向設定用
	D3DXMATRIX  mtxInvWorld;	// ワールドマトリックス逆行列

	// 平行光源の方向ベクトルを計算
	D3DXMatrixInverse(&mtxInvWorld, nullptr, pMtxWorld);	// ワールドマトリックスの逆行列を計算
	D3DXVec4Transform(&setLight, &setLight, &mtxInvWorld);	// マトリックスをベクトルに変換

	// ライトの方向ベクトルを正規化
	dirLight = D3DXVECTOR3(setLight.x, setLight.y, setLight.z);			// 計算したベクトルを代入
	D3DXVec3Normalize(&dirLight, &dirLight);							// ベクトルを正規化
	setLight = D3DXVECTOR4(dirLight.x, dirLight.y, dirLight.z, 0.0f);	// 正規化したベクトルを設定

	// エフェクトに平行光源の方向ベクトルを設定
	GetEffect()->SetVector(m_pDirectLight, &setLight);
}

//============================================================
//	テクスチャの設定処理 (ポインタ)
//============================================================
void CToonShader::SetTexture(const CTexture::STexture& rTexture)
{
	if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用

	// エフェクトにテクスチャの使用状況を設定
	if (rTexture.pTexture == nullptr)
	{ // テクスチャが使用されていない場合

		GetEffect()->SetBool(m_pUseTexture, false);
	}
	else
	{ // テクスチャが使用されている場合

		GetEffect()->SetBool(m_pUseTexture, true);
	}
}

//============================================================
//	テクスチャの設定処理 (インデックス)
//============================================================
void CToonShader::SetTexture(const int nTextureID)
{
	if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用

	// エフェクトにテクスチャの使用状況を設定
	if (nTextureID == NONE_IDX)
	{ // テクスチャが使用されていない場合

		GetEffect()->SetBool(m_pUseTexture, false);
	}
	else if (nTextureID > NONE_IDX)
	{ // テクスチャが使用されている場合

		GetEffect()->SetBool(m_pUseTexture, true);
	}
	else { assert(false); }	// 使用不可インデックス
}

//============================================================
//	マテリアルの設定処理
//============================================================
void CToonShader::SetMaterial(const D3DMATERIAL9& rMaterial)
{
	if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用

	// ポインタを宣言
	LPD3DXEFFECT pEffect = GetEffect();	// エフェクト情報

	// 変数を宣言
	D3DXVECTOR4 diffuse  = D3DXVECTOR4(rMaterial.Diffuse.r,  rMaterial.Diffuse.g,  rMaterial.Diffuse.b,  rMaterial.Diffuse.a);	// 拡散光
	D3DXVECTOR4 ambient  = D3DXVECTOR4(rMaterial.Ambient.r,  rMaterial.Ambient.g,  rMaterial.Ambient.b,  rMaterial.Ambient.a);	// 環境光
	D3DXVECTOR4 emissive = D3DXVECTOR4(rMaterial.Emissive.r, rMaterial.Emissive.g, rMaterial.Emissive.b, rMaterial.Emissive.a);	// 放射光

	// エフェクトにマテリアルの拡散光・環境光・放射光を設定
	pEffect->SetVector(m_pDiffuse,  &diffuse);
	pEffect->SetVector(m_pAmbient,  &ambient);
	pEffect->SetVector(m_pEmissive, &emissive);
}

//============================================================
//	拡散光の設定処理
//============================================================
void CToonShader::SetDiffuse(const D3DXCOLOR& rDiffuse)
{
	if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用

	// 変数を宣言
	D3DXVECTOR4 diffuse = D3DXVECTOR4(rDiffuse.r, rDiffuse.g, rDiffuse.b, rDiffuse.a);	// 拡散光

	// エフェクトに拡散光を設定
	GetEffect()->SetVector(m_pDiffuse, &diffuse);
}

//============================================================
//	環境光の設定処理
//============================================================
void CToonShader::SetAmbient(const D3DXCOLOR& rAmbient)
{
	if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用

	// 変数を宣言
	D3DXVECTOR4 ambient = D3DXVECTOR4(rAmbient.r, rAmbient.g, rAmbient.b, rAmbient.a);	// 環境光

	// エフェクトに環境光を設定
	GetEffect()->SetVector(m_pAmbient, &ambient);
}

//============================================================
//	放射光の設定処理
//============================================================
void CToonShader::SetEmissive(const D3DXCOLOR& rEmissive)
{
	if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用

	// 変数を宣言
	D3DXVECTOR4 emissive = D3DXVECTOR4(rEmissive.r, rEmissive.g, rEmissive.b, rEmissive.a);	// 放射光

	// エフェクトに放射光を設定
	GetEffect()->SetVector(m_pEmissive, &emissive);
}

//============================================================
//	拡散光のみ設定処理
//============================================================
void CToonShader::SetOnlyDiffuse(const D3DXCOLOR& rDiffuse)
{
	// 拡散光の設定
	SetDiffuse(rDiffuse);

	// 環境光の設定
	SetAmbient(XCOL_ABLACK);

	// 放射光の設定
	SetEmissive(XCOL_ABLACK);
}

//============================================================
//	生成処理
//============================================================
CToonShader *CToonShader::Create(void)
{
	// インスタンス使用中
	assert(m_pShader == nullptr);

	// トゥーンシェーダーの生成
	m_pShader = new CToonShader;
	if (m_pShader == nullptr)
	{ // 生成に失敗した場合

		assert(false);
		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// トゥーンシェーダーの初期化
		if (FAILED(m_pShader->Init()))
		{ // 初期化に失敗した場合

			// トゥーンシェーダーの破棄
			SAFE_DELETE(m_pShader);
			return nullptr;
		}

		// 確保したアドレスを返す
		return m_pShader;
	}
}

//============================================================
//	取得処理
//============================================================
CToonShader *CToonShader::GetInstance(void)
{
	// インスタンス未使用
	assert(m_pShader != nullptr);

	// トゥーンシェーダーのポインタを返す
	return m_pShader;
}

//============================================================
//	破棄処理
//============================================================
void CToonShader::Release(void)
{
	// トゥーンシェーダーの終了
	assert(m_pShader != nullptr);
	m_pShader->Uninit();

	// メモリ開放
	SAFE_DELETE(m_pShader);
}

//============================================================
//	トゥーンマップテクスチャの設定処理
//============================================================
void CToonShader::SetToonMapTexture(const ETexture texture)
{
	if (texture > NONE_IDX && texture < TEXTURE_MAX)
	{ // インデックスが範囲内の場合

		if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用
		CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャ情報

		// エフェクトにトゥーンマップ用テクスチャを設定
		GetEffect()->SetTexture(m_pTextureToon, pTexture->GetPtr(pTexture->Regist(TEXTURE_FILE[texture])));
	}
	else { assert(false); }	// 範囲外
}
