//============================================================
//
//	スクリーン処理 [screen.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "screen.h"
#include "manager.h"
#include "renderer.h"

//************************************************************
//	子クラス [CScreen] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CScreen::CScreen()
{

}

//============================================================
//	デストラクタ
//============================================================
CScreen::~CScreen()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CScreen::Init(void)
{
	// オブジェクト2Dの初期化
	if (FAILED(CObject2D::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 位置をスクリーン中央にする
	SetVec3Position(SCREEN_CENT);

	// 大きさをスクリーンサイズにする
	SetVec3Sizing(SCREEN_SIZE);

	// 自動更新・自動描画をOFFにする
	SetEnableUpdate(false);
	SetEnableDraw(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CScreen::Uninit(void)
{
	// オブジェクト2Dの終了
	CObject2D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CScreen::Update(const float fDeltaTime)
{
	// オブジェクト2Dの更新
	CObject2D::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CScreen::Draw(CShader *pShader)
{
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// サンプラーステートを設定
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);	// U方向のラッピングを無効化
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);	// V方向のラッピングを無効化

	// オブジェクト2Dの描画
	CObject2D::Draw(pShader);

	// サンプラーステートを設定
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// U方向のラッピングを有効化
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// V方向のラッピングを有効化
}

//============================================================
//	生成処理
//============================================================
CScreen *CScreen::Create(const int nScreenTexID)
{
	// スクリーンの生成
	CScreen *pScreen = new CScreen;
	if (pScreen == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// スクリーンの初期化
		if (FAILED(pScreen->Init()))
		{ // 初期化に失敗した場合

			// スクリーンの破棄
			SAFE_DELETE(pScreen);
			return nullptr;
		}

		// テクスチャを登録・割当
		pScreen->BindTexture(nScreenTexID);

		// 確保したアドレスを返す
		return pScreen;
	}
}
