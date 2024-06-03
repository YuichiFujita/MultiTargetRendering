//============================================================
//
//	景色処理 [scenery.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "scenery.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャファイル
	{
		"data\\TEXTURE\\scenery000.png",	// 山テクスチャ
		"data\\TEXTURE\\scenery001.png",	// 山テクスチャ
		"data\\TEXTURE\\scenery002.png",	// 山テクスチャ
	};

	const int PRIORITY		= 0;	// 景色の優先順位
	const int ALPHA_NUMREF	= 180;	// αテストの参照値
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CScenery::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CScenery] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CScenery::CScenery() : CObjectMeshCylinder(CObject::LABEL_SCENERY, CObject::DIM_3D, PRIORITY)
{

}

//============================================================
//	デストラクタ
//============================================================
CScenery::~CScenery()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CScenery::Init(void)
{
	// オブジェクトメッシュシリンダーの初期化
	if (FAILED(CObjectMeshCylinder::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャ方向を設定
	SetTexDir(CObjectMeshCylinder::TEXDIR_INSIDE);

	// レンダーステートの情報を取得
	CRenderState *pRenderState = GetRenderState();

	// αテストの設定
	pRenderState->SetAlphaTest(true);			// αテストの有効 / 無効の設定
	pRenderState->SetAlphaFunc(D3DCMP_GREATER);	// αテストの設定
	pRenderState->SetAlphaNumRef(ALPHA_NUMREF);	// αテストの参照値設定

	// ポリゴンの裏面のみを表示状態にする
	pRenderState->SetCulling(D3DCULL_CW);

	// ライティングをOFFにする
	pRenderState->SetLighting(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CScenery::Uninit(void)
{
	// オブジェクトメッシュシリンダーの終了
	CObjectMeshCylinder::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CScenery::Update(const float fDeltaTime)
{
	// オブジェクトメッシュシリンダーの更新
	CObjectMeshCylinder::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CScenery::Draw(CShader *pShader)
{
	// オブジェクトメッシュシリンダーの描画
	CObjectMeshCylinder::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CScenery *CScenery::Create
(
	const ETexture texture,		// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart,		// 分割数
	const POSGRID2& rTexPart,	// テクスチャ分割数
	const float fRadius,		// 半径
	const float fHeight			// 縦幅
)
{
	// 景色の生成
	CScenery *pScenery = new CScenery;
	if (pScenery == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 景色の初期化
		if (FAILED(pScenery->Init()))
		{ // 初期化に失敗した場合

			// 景色の破棄
			SAFE_DELETE(pScenery);
			return nullptr;
		}

		// テクスチャを登録・割当
		pScenery->BindTexture(GET_MANAGER->GetTexture()->Regist(TEXTURE_FILE[texture]));

		// 位置を設定
		pScenery->SetVec3Position(rPos);

		// 向きを設定
		pScenery->SetVec3Rotation(rRot);

		// 色を設定
		pScenery->SetColor(rCol);

		// 半径を設定
		pScenery->SetRadius(fRadius);

		// 縦幅を設定
		pScenery->SetHeight(fHeight);

		// 分割数を設定
		if (FAILED(pScenery->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// 景色の破棄
			SAFE_DELETE(pScenery);
			return nullptr;
		}

		// テクスチャ分割数を設定
		pScenery->SetTexPattern(rTexPart);

		// 確保したアドレスを返す
		return pScenery;
	}
}
