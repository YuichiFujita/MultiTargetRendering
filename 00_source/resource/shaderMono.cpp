//============================================================
//
//	単色描画シェーダーヘッダー [shaderMono.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "shaderMono.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char* EFFECT_FX = "data\\SHADER\\shaderMono.fx";	// 単色描画シェーダーのエフェクトファイル
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CMonoShader *CMonoShader::m_pShader = nullptr;	// シェーダー情報

//************************************************************
//	親クラス [CMonoShader] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CMonoShader::CMonoShader() :
	m_pColDraw	(nullptr)	// ピクセル描画色

{

}

//============================================================
//	デストラクタ
//============================================================
CMonoShader::~CMonoShader()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CMonoShader::Init(void)
{
	// 変数を宣言
	D3DCAPS9 caps;	// ハードウェア機能
	HRESULT hr;		// 異常終了の確認用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイス情報
	LPD3DXBUFFER pError  = nullptr;	// コンパイルエラー情報
	LPD3DXEFFECT pEffect = nullptr;	// エフェクト設定用

	// メンバ変数を初期化
	m_pColDraw = nullptr;	// ピクセル描画色

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
			m_pColDraw = pEffect->GetParameterByName(nullptr, "g_colDraw");	// ピクセル描画色
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
void CMonoShader::Uninit(void)
{
	// シェーダーの終了
	CShader::Uninit();
}

//============================================================
//	ピクセル描画色の設定処理
//============================================================
void CMonoShader::SetColor(const D3DXCOLOR& rCol)
{
	if (!IsEffectOK()) { assert(false); return; }	// エフェクト未使用

	// 変数を宣言
	D3DXVECTOR4 col = D3DXVECTOR4(rCol.r, rCol.g, rCol.b, rCol.a);	// ピクセル描画色

	// エフェクトにピクセル描画色を設定
	GetEffect()->SetVector(m_pColDraw, &col);
}

//============================================================
//	生成処理
//============================================================
CMonoShader *CMonoShader::Create(void)
{
	// インスタンス使用中
	assert(m_pShader == nullptr);

	// 単色描画シェーダーの生成
	m_pShader = new CMonoShader;
	if (m_pShader == nullptr)
	{ // 生成に失敗した場合

		assert(false);
		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 単色描画シェーダーの初期化
		if (FAILED(m_pShader->Init()))
		{ // 初期化に失敗した場合

			// 単色描画シェーダーの破棄
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
CMonoShader *CMonoShader::GetInstance(void)
{
	// インスタンス未使用
	assert(m_pShader != nullptr);

	// 単色描画シェーダーのポインタを返す
	return m_pShader;
}

//============================================================
//	破棄処理
//============================================================
void CMonoShader::Release(void)
{
	// 単色描画シェーダーの終了
	assert(m_pShader != nullptr);
	m_pShader->Uninit();

	// メモリ開放
	SAFE_DELETE(m_pShader);
}
