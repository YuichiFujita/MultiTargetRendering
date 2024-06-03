//============================================================
//
//	モデルUI処理 [modelUI.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "modelUI.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "model.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 5;	// モデルUI表示の優先順位
}

//************************************************************
//	子クラス [CModelUI] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CModelUI::CModelUI() : CObjectModel(CObject::LABEL_NONE, CObject::DIM_2D, PRIORITY)
{

}

//============================================================
//	デストラクタ
//============================================================
CModelUI::~CModelUI()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CModelUI::Init(void)
{
	// オブジェクトモデルの初期化
	if (FAILED(CObjectModel::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CModelUI::Uninit(void)
{
	// オブジェクトモデルの終了
	CObjectModel::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CModelUI::Update(const float fDeltaTime)
{
	// オブジェクトモデルの更新
	CObjectModel::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CModelUI::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DVIEWPORT9 viewportDef;	// カメラのビューポート保存用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 現在のビューポートを取得
	pDevice->GetViewport(&viewportDef);

	// カメラの設定
	GET_MANAGER->GetCamera()->SetCamera(CCamera::TYPE_MODELUI);

	// オブジェクトモデルの描画
	CObjectModel::Draw(pShader);

	// カメラの設定を元に戻す
	GET_MANAGER->GetCamera()->SetCamera(CCamera::TYPE_MAIN);

	// ビューポートを元に戻す
	pDevice->SetViewport(&viewportDef);
}

//============================================================
//	生成処理
//============================================================
CModelUI *CModelUI::Create
( // 引数
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR3& rScale	// 大きさ
)
{
	// モデルUIの生成
	CModelUI *pModelUI = new CModelUI;
	if (pModelUI == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// モデルUIの初期化
		if (FAILED(pModelUI->Init()))
		{ // 初期化に失敗した場合

			// モデルUIの破棄
			SAFE_DELETE(pModelUI);
			return nullptr;
		}

		// 位置を設定
		pModelUI->SetVec3Position(rPos);

		// 向きを設定
		pModelUI->SetVec3Rotation(rRot);

		// 拡大率を設定
		pModelUI->SetVec3Scaling(rScale);

		// 確保したアドレスを返す
		return pModelUI;
	}
}
