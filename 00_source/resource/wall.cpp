//============================================================
//
//	壁処理 [wall.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "wall.h"
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
		nullptr,	// 通常テクスチャ
	};

	const int PRIORITY = 0;	// 壁の優先順位
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CWall::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CWall] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CWall::CWall() : CObjectMeshWall(CObject::LABEL_WALL, CObject::DIM_3D, PRIORITY)
{

}

//============================================================
//	デストラクタ
//============================================================
CWall::~CWall()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CWall::Init(void)
{
	// オブジェクトメッシュウォールの初期化
	if (FAILED(CObjectMeshWall::Init()))
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
void CWall::Uninit(void)
{
	// オブジェクトメッシュウォールの終了
	CObjectMeshWall::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CWall::Update(const float fDeltaTime)
{
	// オブジェクトメッシュウォールの更新
	CObjectMeshWall::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CWall::Draw(CShader *pShader)
{
	// オブジェクトメッシュウォールの描画
	CObjectMeshWall::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CWall *CWall::Create
(
	const ETexture texture,		// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR2& rSize,	// 大きさ
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart		// 分割数
)
{
	// 壁の生成
	CWall *pWall = new CWall;
	if (pWall == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 壁の初期化
		if (FAILED(pWall->Init()))
		{ // 初期化に失敗した場合

			// 壁の破棄
			SAFE_DELETE(pWall);
			return nullptr;
		}

		// テクスチャを登録・割当
		pWall->BindTexture(GET_MANAGER->GetTexture()->Regist(TEXTURE_FILE[texture]));

		// 位置を設定
		pWall->SetVec3Position(rPos);

		// 向きを設定
		pWall->SetVec3Rotation(rRot);

		// 大きさを設定
		pWall->SetVec2Sizing(rSize);

		// 色を設定
		pWall->SetColor(rCol);

		// 分割数を設定
		if (FAILED(pWall->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// 壁の破棄
			SAFE_DELETE(pWall);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pWall;
	}
}
