//============================================================
//
//	エフェクト3D処理 [effect3D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "effect3D.h"
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
		"data\\TEXTURE\\EFFECT\\effect001.png",	// バブルテクスチャ
		"data\\TEXTURE\\EFFECT\\effect002.tga",	// 煙テクスチャ
		"data\\TEXTURE\\EFFECT\\effect003.jpg",	// 回復テクスチャ
		"data\\TEXTURE\\EFFECT\\effect004.png",	// 葉っぱテクスチャ
		"data\\TEXTURE\\EFFECT\\effect005.jpg",	// かけらテクスチャ (小)
		"data\\TEXTURE\\EFFECT\\effect006.jpg",	// かけらテクスチャ (中)
		"data\\TEXTURE\\EFFECT\\effect007.jpg",	// かけらテクスチャ (大)
		"data\\TEXTURE\\EFFECT\\effect008.jpg",	// 炎テクスチャ
	};

	const int EFF_PRIO	= 3;	// エフェクト3Dの優先順位
	const int PART_PRIO	= 4;	// パーティクル3Dの優先順位
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CEffect3D::TYPE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CEffect3D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEffect3D::CEffect3D(const EType type, const CObject::ELabel label) : CObjectBillboard(CObject::LABEL_EFFECT, CObject::DIM_3D, (label == LABEL_EFFECT) ? EFF_PRIO : PART_PRIO),
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
CEffect3D::~CEffect3D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEffect3D::Init(void)
{
	// メンバ変数を初期化
	m_move		= VEC3_ZERO;	// 移動量
	m_nLife		= 0;			// 寿命
	m_fSubSize	= 0.0f;			// 大きさの減算量
	m_fSubAlpha	= 0.0f;			// 透明度の減算量
	m_blend		= CRenderState::BLEND_ADD;	// αブレンド状況

	// オブジェクトビルボードの初期化
	if (FAILED(CObjectBillboard::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// レンダーステートの情報を取得
	CRenderState *pRenderState = GetRenderState();

	// Zテストの設定
	pRenderState->SetZFunc(D3DCMP_ALWAYS);

	// Zバッファの使用状況の設定
	pRenderState->SetZUpdate(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEffect3D::Uninit(void)
{
	// オブジェクトビルボードの終了
	CObjectBillboard::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEffect3D::Update(const float fDeltaTime)
{
	// 変数を宣言
	D3DXVECTOR3 pos  = GetVec3Position();	// 位置
	D3DXVECTOR3 rot  = GetVec3Rotation();	// 向き
	D3DXVECTOR3 size = GetVec3Sizing();		// 大きさ
	D3DXCOLOR   col  = GetColor();			// 色
	float fRadius    = size.x;				// 半径

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
	CObjectBillboard::SetVec3Position(pos);

	// 向きを設定
	CObjectBillboard::SetVec3Rotation(rot);

	// 大きさを設定
	CObjectBillboard::SetVec3Sizing(D3DXVECTOR3(fRadius, fRadius, 0.0f));

	// 色を設定
	CObjectBillboard::SetColor(col);

	// オブジェクトビルボードの更新
	CObjectBillboard::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CEffect3D::Draw(CShader *pShader)
{
	// オブジェクトビルボードの描画
	CObjectBillboard::Draw(pShader);
}

//============================================================
//	αブレンドの設定処理
//============================================================
void CEffect3D::SetAlphaBlend(const CRenderState::EBlend blend)
{
	// 引数のαブレンドを設定
	m_blend = blend;

	// αブレンドを変更
	GetRenderState()->SetAlphaBlend(blend);
}

//============================================================
//	生成処理
//============================================================
CEffect3D *CEffect3D::Create
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
	// エフェクト3Dの生成
	CEffect3D *pEffect3D = new CEffect3D(type, label);
	if (pEffect3D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// エフェクト3Dの初期化
		if (FAILED(pEffect3D->Init()))
		{ // 初期化に失敗した場合

			// エフェクト3Dの破棄
			SAFE_DELETE(pEffect3D);
			return nullptr;
		}

		// テクスチャを登録・割当
		pEffect3D->BindTexture(GET_MANAGER->GetTexture()->Regist(TEXTURE_FILE[type]));

		// 位置を設定
		pEffect3D->SetVec3Position(rPos);

		// 向きを設定
		pEffect3D->SetVec3Rotation(rRot);

		// 大きさを設定
		pEffect3D->SetVec3Sizing(D3DXVECTOR3(fRadius, fRadius, 0.0f));

		// 色を設定
		pEffect3D->SetColor(rCol);

		// αブレンドを設定
		pEffect3D->SetAlphaBlend(blend);

		// 引数の情報を設定
		pEffect3D->m_move		= rMove;		// 移動量
		pEffect3D->m_nLife		= nLife;		// 寿命
		pEffect3D->m_fSubSize	= fSubSize;		// 大きさの減算量
		pEffect3D->m_fSubAlpha	= 1.0f / nLife;	// 透明度の減算量

		// 確保したアドレスを返す
		return pEffect3D;
	}
}
