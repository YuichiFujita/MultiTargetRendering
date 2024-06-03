//============================================================
//
//	影処理 [shadow.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "shadow.h"
#include "manager.h"
#include "scene.h"
#include "renderer.h"
#include "texture.h"
#include "collision.h"
#include "field.h"
#include "stage.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャファイル
	{
		"data\\TEXTURE\\shadow000.jpg",	// 丸影テクスチャ
	};

	const int	PRIORITY		= 1;		// 影の優先順位
	const float	PLUS_POSY		= 0.01f;	// ちらつき防止用の縦座標加算量
	const float	MAX_DIS_HEIGHT	= 200.0f;	// 影と親の縦の距離の最大値
	const float	MAX_PLUS_SIZE	= 120.0f;	// 影の大きさ加算量の最大値
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CShadow::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CShadow] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CShadow::CShadow(const float fMinAlpha, const float fMaxAlpha) : CObject3D(CObject::LABEL_NONE, CObject::DIM_3D, PRIORITY),
	m_pParentObject	(nullptr),		// 親オブジェクト
	m_sizeOrigin	(VEC3_ZERO),	// 元の大きさ
	m_fMinAlpha		(fMinAlpha),	// 透明度の最小値定数
	m_fMaxAlpha		(fMaxAlpha)		// 透明度の最大値定数
{

}

//============================================================
//	デストラクタ
//============================================================
CShadow::~CShadow()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CShadow::Init(void)
{
	// メンバ変数を初期化
	m_pParentObject	= nullptr;		// 親オブジェクト
	m_sizeOrigin	= VEC3_ZERO;	// 元の大きさ

	// オブジェクト3Dの初期化
	if (FAILED(CObject3D::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// レンダーステートの情報を取得
	CRenderState *pRenderState = GetRenderState();

	// Zテストの設定
	pRenderState->SetZFunc(D3DCMP_LESSEQUAL);

	// Zバッファの使用状況の設定
	pRenderState->SetZUpdate(false);

	// αブレンドの設定
	pRenderState->SetAlphaBlend(CRenderState::BLEND_SUB);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CShadow::Uninit(void)
{
	// 親オブジェクトの削除
	DeleteObjectParent();

	// オブジェクト3Dの終了
	CObject3D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CShadow::Update(const float fDeltaTime)
{
	// 描画情報を設定
	SetDrawInfo();

	// オブジェクト3Dの更新
	CObject3D::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CShadow::Draw(CShader *pShader)
{
	// オブジェクト3Dの描画
	CObject3D::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CShadow *CShadow::Create
(
	const ETexture texture,		// 種類
	const D3DXVECTOR3& rSize,	// 大きさ
	CObject *pObject,			// 親オブジェクト
	const float fMinAlpha,		// 透明度の最小値
	const float fMaxAlpha		// 透明度の最大値
)
{
	// 影の生成
	CShadow *pShadow = new CShadow(fMinAlpha, fMaxAlpha);
	if (pShadow == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 影の初期化
		if (FAILED(pShadow->Init()))
		{ // 初期化に失敗した場合

			// 影の破棄
			SAFE_DELETE(pShadow);
			return nullptr;
		}

		// テクスチャを登録・割当
		pShadow->BindTexture(GET_MANAGER->GetTexture()->Regist(TEXTURE_FILE[texture]));

		// 元の大きさを設定
		pShadow->SetSizingOrigin(rSize);

		// 親オブジェクトを設定
		pShadow->SetParentObject(pObject);

		// 描画情報を設定
		pShadow->SetDrawInfo();

		// 確保したアドレスを返す
		return pShadow;
	}
}

//============================================================
//	元の大きさの設定処理
//============================================================
void CShadow::SetSizingOrigin(const D3DXVECTOR3& rSize)
{
	// 引数の大きさを代入
	m_sizeOrigin = rSize;

	// 大きさの設定
	CObject3D::SetVec3Sizing(rSize);
}

//============================================================
//	描画情報の設定処理
//============================================================
void CShadow::SetDrawInfo(void)
{
	if (m_pParentObject != nullptr)
	{ // 親オブジェクトが使用されていた場合

		// 変数を宣言
		D3DXVECTOR3 posParent = m_pParentObject->GetVec3Position();	// 親オブジェクト位置
		D3DXVECTOR3 posShadow  = VEC3_ZERO;	// 影位置
		D3DXVECTOR3 sizeShadow = VEC3_ZERO;	// 影大きさ
		float fDis = 0.0f;		// 影と親の距離
		float fAlpha = 0.0f;	// 影の透明度

		// 描画位置の設定
		posShadow = SetDrawPosition();

		// 影と親の縦座標の距離を求める
		fDis = fabsf(posParent.y - posShadow.y);		// 縦の距離を求める
		useful::LimitNum(fDis, 0.0f, MAX_DIS_HEIGHT);	// 縦の距離を制限
		fDis *= 1.0f / MAX_DIS_HEIGHT;					// 距離を割合化

		// 影の大きさを求める
		sizeShadow = D3DXVECTOR3(m_sizeOrigin.x + (MAX_PLUS_SIZE * fDis), 0.0f, m_sizeOrigin.z + (MAX_PLUS_SIZE * fDis));

		// α値を求める
		fAlpha = fabsf(fDis - 1.0f);	// α値を設定
		useful::LimitNum(fAlpha, m_fMinAlpha, m_fMaxAlpha);	// α値を制限

		// 影の描画情報を設定
		SetVec3Position(posShadow);	// 位置
		SetVec3Sizing(sizeShadow);	// 大きさ
		SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha));	// 色
	}
}

//============================================================
//	描画位置の設定処理
//============================================================
D3DXVECTOR3 CShadow::SetDrawPosition(void)
{
	// 変数を宣言
	D3DXVECTOR3 posParent = m_pParentObject->GetVec3Position();	// 親オブジェクト位置
	D3DXVECTOR3 posShadow = VEC3_ZERO;	// 影位置

	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージの情報
	if (pStage == nullptr)
	{ // ステージが存在しない場合

		// 関数を抜ける
		return VEC3_ZERO;
	}

	// 影の位置を求める
	posShadow = posParent;	// 親オブジェクトの座標代入

	if (pStage->IsFieldPositionRange(posParent))
	{ // 地面の範囲内の場合

		// 高さを地面に設定
		posShadow.y = pStage->GetFieldPositionHeight(posParent) + PLUS_POSY;
	}
	else
	{ // 全ての範囲外の場合

		// 高さを制限位置に設定
		posShadow.y = pStage->GetStageLimit().fField + PLUS_POSY;
	}

	// 影位置を返す
	return posShadow;
}

//============================================================
//	親オブジェクトの設定処理
//============================================================
void CShadow::SetParentObject(CObject *pObject)
{
	// 引数のオブジェクトを設定
	m_pParentObject = pObject;
}

//============================================================
//	親オブジェクトの削除処理
//============================================================
void CShadow::DeleteObjectParent(void)
{
	// 親オブジェクトをnullptrにする
	m_pParentObject = nullptr;
}
