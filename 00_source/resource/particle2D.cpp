//============================================================
//
//	パーティクル2D処理 [particle2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "particle2D.h"
#include "effect2D.h"
#include "renderState.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int SET_LIFE[] =	// 種類ごとの寿命
	{
		0,	// なし
		18,	// ダメージ
		1,	// アイテム
		2,	// アイテム取得
	};

	// ダメージ
	namespace damage
	{
		const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// ダメージのαブレンド

		const float	MOVE		= 3.0f;		// ダメージの移動量
		const int	SPAWN		= 35;		// ダメージの生成数
		const int	EFF_LIFE	= 120;		// ダメージの寿命
		const float	SIZE		= 50.0f;	// ダメージの大きさ
		const float	SUB_SIZE	= 1.8f;		// ダメージの半径の減算量
	}

	// アイテム
	namespace item
	{

		const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// アイテムのαブレンド

		const float	MOVE		= 2.0f;		// アイテムの移動量
		const int	SPAWN		= 6;		// アイテムの生成数
		const int	EFF_LIFE	= 80;		// アイテムの寿命
		const float	SIZE		= 30.0f;	// アイテムの大きさ
		const float	SUB_SIZE	= 1.2f;		// アイテムの半径の減算量
	}

	// アイテム取得
	namespace itemGet
	{
		const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// アイテム取得のαブレンド

		const float	MOVE		= 3.0f;		// アイテム取得の移動量
		const int	SPAWN		= 35;		// アイテム取得の生成数
		const int	EFF_LIFE	= 120;		// アイテム取得の寿命
		const float	SIZE		= 50.0f;	// アイテム取得の大きさ
		const float	SUB_SIZE	= 1.8f;		// アイテム取得の半径の減算量
	}
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(SET_LIFE) == CParticle2D::TYPE_MAX, "ERROR : Type Count Mismatch");

//************************************************************
//	子クラス [CParticle2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CParticle2D::CParticle2D() : CObject(CObject::LABEL_PARTICLE, CObject::DIM_2D),
	m_pos	(VEC3_ZERO),	// 位置
	m_col	(XCOL_WHITE),	// 色
	m_type	(TYPE_NONE),	// 種類
	m_nLife	(0)				// 寿命
{

}

//============================================================
//	デストラクタ
//============================================================
CParticle2D::~CParticle2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CParticle2D::Init(void)
{
	// メンバ変数を初期化
	m_pos	= VEC3_ZERO;	// 位置
	m_col	= XCOL_WHITE;	// 色
	m_type	= TYPE_NONE;	// 種類
	m_nLife	= 0;			// 寿命

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CParticle2D::Uninit(void)
{
	// パーティクル2Dオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CParticle2D::Update(const float fDeltaTime)
{
	if (m_nLife > 0)
	{ // 寿命がある場合

		// 寿命を減算
		m_nLife--;
	}
	else
	{ // 寿命がない場合

		// パーティクル2Dオブジェクトの終了
		Uninit();

		// 関数を抜ける
		return;
	}

	switch (m_type)
	{ // 種類ごとの処理
	case TYPE_DAMAGE:

		// ダメージパーティクル2D
		Damage(m_pos, m_col);

		break;

	case TYPE_ITEM:

		// アイテムパーティクル2D
		Item(m_pos, m_col);

		break;

	case TYPE_GETITEM:

		// アイテム取得パーティクル2D
		GetItem(m_pos, m_col);

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	描画処理
//============================================================
void CParticle2D::Draw(CShader * /*pShader*/)
{

}

//============================================================
//	位置の設定処理
//============================================================
void CParticle2D::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 位置を設定
	m_pos = rPos;
}

//============================================================
//	生成処理
//============================================================
CParticle2D *CParticle2D::Create(const EType type, const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol)
{
	// パーティクル2Dの生成
	CParticle2D *pParticle2D = new CParticle2D;
	if (pParticle2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// パーティクル2Dの初期化
		if (FAILED(pParticle2D->Init()))
		{ // 初期化に失敗した場合

			// パーティクル2Dの破棄
			SAFE_DELETE(pParticle2D);
			return nullptr;
		}

		// 種類を設定
		pParticle2D->SetType(type);

		// 位置を設定
		pParticle2D->SetVec3Position(rPos);

		// 色を設定
		pParticle2D->SetColor(rCol);

		// 確保したアドレスを返す
		return pParticle2D;
	}
}

//============================================================
//	色の設定処理
//============================================================
void CParticle2D::SetColor(const D3DXCOLOR& rCol)
{
	// 色を設定
	m_col = rCol;
}

//============================================================
//	種類の設定処理
//============================================================
void CParticle2D::SetType(const EType type)
{
	if (type > NONE_IDX && type < TYPE_MAX)
	{ // 種類が正規の場合

		// 種類を設定
		m_type = type;

		// 寿命を設定
		m_nLife = SET_LIFE[(int)type];
	}
	else { assert(false); }
}

//============================================================
//	ダメージパーティクル2D処理
//============================================================
void CParticle2D::Damage(const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol)
{
	// 変数を宣言
	D3DXVECTOR3 move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用

	if ((m_nLife + 1) % 9 == 0)
	{ // 寿命が9の倍数の場合

		for (int nCntPart = 0; nCntPart < damage::SPAWN; nCntPart++)
		{ // 生成されるエフェクト数分繰り返す

			// ベクトルをランダムに設定
			move.x = sinf(useful::RandomRot());
			move.y = cosf(useful::RandomRot());
			move.z = 0.0f;

			// ベクトルを正規化
			D3DXVec3Normalize(&move, &move);

			// 移動量を設定
			move.x *= damage::MOVE;
			move.y *= damage::MOVE;
			move.z = 0.0f;

			// 向きを設定
			rot = VEC3_ZERO;

			// エフェクト2Dオブジェクトの生成
			CEffect2D::Create
			( // 引数
				rPos,					// 位置
				damage::SIZE,			// 半径
				CEffect2D::TYPE_NORMAL,	// テクスチャ
				damage::EFF_LIFE,		// 寿命
				move,					// 移動量
				rot,					// 向き
				rCol,					// 色
				damage::SUB_SIZE,		// 半径の減算量
				damage::BLEND,			// 加算合成状況
				LABEL_PARTICLE			// オブジェクトラベル
			);
		}
	}
}

//============================================================
//	アイテムパーティクル2D処理
//============================================================
void CParticle2D::Item(const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol)
{
	// 変数を宣言
	D3DXVECTOR3 move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用

	for (int nCntPart = 0; nCntPart < item::SPAWN; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		move.x = sinf(useful::RandomRot());
		move.y = cosf(useful::RandomRot());
		move.z = 0.0f;

		// ベクトルを正規化
		D3DXVec3Normalize(&move, &move);

		// 移動量を設定
		move.x *= item::MOVE;
		move.y *= item::MOVE;
		move.z = 0.0f;

		// 向きを設定
		rot = VEC3_ZERO;

		// エフェクト2Dオブジェクトの生成
		CEffect2D::Create
		( // 引数
			rPos,					// 位置
			item::SIZE,				// 半径
			CEffect2D::TYPE_NORMAL,	// テクスチャ
			item::EFF_LIFE,			// 寿命
			move,					// 移動量
			rot,					// 向き
			rCol,					// 色
			item::SUB_SIZE,			// 半径の減算量
			item::BLEND,			// 加算合成状況
			LABEL_PARTICLE			// オブジェクトラベル
		);
	}
}

//============================================================
//	アイテム取得パーティクル2D処理
//============================================================
void CParticle2D::GetItem(const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol)
{
	// 変数を宣言
	D3DXVECTOR3 move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用

	for (int nCntPart = 0; nCntPart < itemGet::SPAWN; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		move.x = sinf(useful::RandomRot());
		move.y = cosf(useful::RandomRot());
		move.z = 0.0f;

		// ベクトルを正規化
		D3DXVec3Normalize(&move, &move);

		// 移動量を設定
		move.x *= itemGet::MOVE;
		move.y *= itemGet::MOVE;
		move.z = 0.0f;

		// 向きを設定
		rot = VEC3_ZERO;

		// エフェクト2Dオブジェクトの生成
		CEffect2D::Create
		( // 引数
			rPos,					// 位置
			itemGet::SIZE,			// 半径
			CEffect2D::TYPE_NORMAL,	// テクスチャ
			itemGet::EFF_LIFE,		// 寿命
			move,					// 移動量
			rot,					// 向き
			rCol,					// 色
			itemGet::SUB_SIZE,		// 半径の減算量
			itemGet::BLEND,			// 加算合成状況
			LABEL_PARTICLE			// オブジェクトラベル
		);
	}
}
