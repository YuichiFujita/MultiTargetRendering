//============================================================
//
//	エフェクト2D処理 [effect2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "effect2D.h"
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
		"data\\TEXTURE\\EFFECT\\effect000.jpg",	// 通常テクスチャ
	};

	const int EFF_PRIO	= 3;	// エフェクト2Dの優先順位
	const int PART_PRIO	= 4;	// パーティクル2Dの優先順位
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CEffect2D::TYPE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CEffect2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEffect2D::CEffect2D(const EType type, const CObject::ELabel label) : CObject2D(CObject::LABEL_EFFECT, CObject::DIM_2D, (label == LABEL_EFFECT) ? EFF_PRIO : PART_PRIO),
	m_type		(type),			// 種類
	m_move		(VEC3_ZERO),	// 移動量
	m_nLife		(0),			// 寿命
	m_fSubSize	(0.0f),			// 大きさの減算量
	m_fSubAlpha	(0.0f),			// 透明度の減算量
	m_blend		(CRenderState::BLEND_ADD)	// αブレンド状況
{

}

//============================================================
//	デストラクタ
//============================================================
CEffect2D::~CEffect2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEffect2D::Init(void)
{
	// メンバ変数を初期化
	m_move		= VEC3_ZERO;	// 移動量
	m_nLife		= 0;			// 寿命
	m_fSubSize	= 0.0f;			// 大きさの減算量
	m_fSubAlpha	= 0.0f;			// 透明度の減算量
	m_blend		= CRenderState::BLEND_ADD;	// αブレンド状況

	// オブジェクト2Dの初期化
	if (FAILED(CObject2D::Init()))
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
void CEffect2D::Uninit(void)
{
	// オブジェクト2Dの終了
	CObject2D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEffect2D::Update(const float fDeltaTime)
{
	// 変数を宣言
	D3DXVECTOR3 pos  = GetVec3Position();	// 位置
	D3DXVECTOR3 rot  = GetVec3Rotation();	// 向き
	D3DXVECTOR3 size = GetVec3Sizing();		// 大きさ
	D3DXCOLOR   col  = GetColor();			// 色
	float fRadius = size.x;					// 半径

	if (m_nLife <= 0		// 寿命を迎えた
	||  fRadius <= 0.0f)	// 半径が0.0f以下
	{ // 上記のどれかになった場合

		// オブジェクトを破棄
		Uninit();

		// 関数を抜ける
		return;
	}

	// 移動量を加算
	pos += m_move;

	// 寿命を減算
	m_nLife--;

	// 半径を減算
	fRadius -= m_fSubSize;
	if (fRadius < 0.0f)
	{ // 半径が0.0fより小さい場合

		// 半径を補正
		fRadius = 0.0f;
	}

	// α値を減算
	col.a -= m_fSubAlpha;
	useful::LimitNum(col.a, 0.0f, 1.0f);	// α値制限

	// 位置を設定
	CObject2D::SetVec3Position(pos);

	// 向きを設定
	CObject2D::SetVec3Rotation(rot);

	// 大きさを設定
	CObject2D::SetVec3Sizing(D3DXVECTOR3(fRadius, fRadius, 0.0f));

	// 色を設定
	CObject2D::SetColor(col);

	// オブジェクト2Dの更新
	CObject2D::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CEffect2D::Draw(CShader *pShader)
{
	// オブジェクト2Dの描画
	CObject2D::Draw(pShader);
}

//============================================================
//	αブレンドの設定処理
//============================================================
void CEffect2D::SetAlphaBlend(const CRenderState::EBlend blend)
{
	// 引数のαブレンドを設定
	m_blend = blend;

	// αブレンドを変更
	GetRenderState()->SetAlphaBlend(blend);
}

//============================================================
//	生成処理
//============================================================
CEffect2D *CEffect2D::Create
(
	const D3DXVECTOR3& rPos,			// 位置
	const float fRadius,				// 半径
	const EType type,					// テクスチャ
	const int nLife,					// 寿命
	const D3DXVECTOR3& rMove,			// 移動量
	const D3DXVECTOR3& rRot,			// 向き
	const D3DXCOLOR& rCol,				// 色
	const float fSubSize,				// 半径の減算量
	const CRenderState::EBlend blend,	// αブレンド状況
	const CObject::ELabel label			// オブジェクトラベル
)
{
	// エフェクト2Dの生成
	CEffect2D *pEffect2D = new CEffect2D(type, label);
	if (pEffect2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// エフェクト2Dの初期化
		if (FAILED(pEffect2D->Init()))
		{ // 初期化に失敗した場合

			// エフェクト2Dの破棄
			SAFE_DELETE(pEffect2D);
			return nullptr;
		}

		// テクスチャを登録・割当
		pEffect2D->BindTexture(GET_MANAGER->GetTexture()->Regist(TEXTURE_FILE[type]));

		// 位置を設定
		pEffect2D->SetVec3Position(rPos);

		// 向きを設定
		pEffect2D->SetVec3Rotation(rRot);

		// 大きさを設定
		pEffect2D->SetVec3Sizing(D3DXVECTOR3(fRadius, fRadius, 0.0f));

		// 色を設定
		pEffect2D->SetColor(rCol);

		// αブレンドを設定
		pEffect2D->SetAlphaBlend(blend);

		// 引数の情報を設定
		pEffect2D->m_move		= rMove;		// 移動量
		pEffect2D->m_nLife		= nLife;		// 寿命
		pEffect2D->m_fSubSize	= fSubSize;		// 大きさの減算量
		pEffect2D->m_fSubAlpha	= 1.0f / nLife;	// 透明度の減算量

		// 確保したアドレスを返す
		return pEffect2D;
	}
}
