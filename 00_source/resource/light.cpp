//============================================================
//
//	ライト処理 [light.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "light.h"
#include "manager.h"
#include "renderer.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const D3DXCOLOR DIFFUSE[] =	// 設定用拡散光カラー
	{
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
		D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f),
		D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f),
		D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f),
	};

	const D3DXVECTOR3 DIRECTION[] =	// 設定用方向ベクトル
	{
		D3DXVECTOR3( 0.22f, -0.97f,  0.54f),
		D3DXVECTOR3(-0.38f,  0.78f, -0.54f),
		D3DXVECTOR3( 0.89f, -0.21f,  0.44f),
		D3DXVECTOR3(-0.96f,  0.15f, -0.44f),
	};
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(DIFFUSE)   == light::MAX_NUM, "ERROR : Light Count Mismatch");
static_assert(NUM_ARRAY(DIRECTION) == light::MAX_NUM, "ERROR : Light Count Mismatch");

//************************************************************
//	親クラス [CLight] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CLight::CLight()
{
	// メンバ変数をクリア
	memset(&m_aLight[0], 0, sizeof(m_aLight));	// ライトの情報
}

//============================================================
//	デストラクタ
//============================================================
CLight::~CLight()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CLight::Init(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	for (int nCntLight = 0; nCntLight < light::MAX_NUM; nCntLight++)
	{ // 使用するライト数分繰り返す

		// 変数を宣言
		D3DXCOLOR	diffuse		= DIFFUSE[nCntLight];	// 設定用拡散光カラー
		D3DXVECTOR3	direction	= DIRECTION[nCntLight];	// 設定用方向ベクトル

		// 構造体の要素をクリア
		ZeroMemory(&m_aLight[nCntLight], sizeof(D3DLIGHT9));

		// ライトの種類を設定
		m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

		// ライトの拡散光を設定
		m_aLight[nCntLight].Diffuse = diffuse;

		// ライトの方向を設定
		D3DXVec3Normalize(&direction, &direction);	// ベクトルを正規化
		m_aLight[nCntLight].Direction = direction;

		// ライトを設定する
		pDevice->SetLight(nCntLight, &m_aLight[nCntLight]);

		// ライトを有効にする
		pDevice->LightEnable(nCntLight, TRUE);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CLight::Uninit(void)
{

}

//============================================================
//	更新処理
//============================================================
void CLight::Update(const float fDeltaTime)
{

}

//============================================================
//	ライト取得処理
//============================================================
D3DLIGHT9 CLight::GetLight(const int nID)
{
	if (nID > NONE_IDX && nID < light::MAX_NUM)
	{ // 要素数の範囲内の場合

		// ライトの情報を返す
		return m_aLight[nID];
	}
	else { assert(false); return m_aLight[0]; }	// 範囲外
}

//============================================================
//	生成処理
//============================================================
CLight *CLight::Create(void)
{
	// ライトの生成
	CLight *pLight = new CLight;
	if (pLight == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// ライトの初期化
		if (FAILED(pLight->Init()))
		{ // 初期化に失敗した場合

			// ライトの破棄
			SAFE_DELETE(pLight);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pLight;
	}
}

//============================================================
//	破棄処理
//============================================================
void CLight::Release(CLight *&prLight)
{
	// ライトの終了
	assert(prLight != nullptr);
	prLight->Uninit();

	// メモリ開放
	SAFE_DELETE(prLight);
}
