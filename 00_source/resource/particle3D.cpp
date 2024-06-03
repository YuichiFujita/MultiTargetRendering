//============================================================
//
//	パーティクル3D処理 [particle3D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "particle3D.h"
#include "effect3D.h"
#include "renderState.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int SET_LIFE[] =	// 種類ごとの寿命
	{
		0,	// なし
		24,	// ダメージ
		1,	// 回復
		1,	// 炎
		1,	// 小爆発
		1,	// 大爆発
		1,	// プレイヤーダメージ
	};

	// ダメージ爆発
	namespace damage
	{
		const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// ダメージ・バブル爆発のαブレンド

		const float	MOVE		= 5.0f;		// ダメージ・バブル爆発の移動量
		const int	SPAWN		= 35;		// ダメージ・バブル爆発の生成数
		const int	EFF_LIFE	= 120;		// ダメージ・バブル爆発の寿命
		const float	SIZE		= 80.0f;	// ダメージ・バブル爆発の大きさ
		const float	SUB_SIZE	= 2.4f;		// ダメージ・バブル爆発の半径の減算量
	}

	// 回復
	namespace heal
	{
		const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// 回復のαブレンド

		const float	POSGAP		= 24.0f;	// 回復の位置ずれ量
		const float	MOVE		= 1.2f;		// 回復の移動量
		const int	SPAWN		= 6;		// 回復の生成数
		const int	EFF_LIFE	= 50;		// 回復の寿命
		const float	SIZE		= 55.0f;	// 回復の大きさ
		const float	SUB_SIZE	= 0.5f;		// 回復の半径の減算量
	}

	// 炎
	namespace fire
	{
		const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// 炎のαブレンド
		const D3DXCOLOR COL = D3DXCOLOR(1.0f, 0.35f, 0.1f, 1.0f);	// 炎の色

		const float	POSGAP		= 30.0f;	// 炎の位置ずれ量
		const float	MOVE		= -2.0f;	// 炎の移動量
		const int	SPAWN		= 3;		// 炎の生成数
		const int	EFF_LIFE	= 6;		// 炎の寿命
		const float	INIT_RAD	= 150.0f;	// 炎の半径
		const float	INIT_SUBRAD	= 5.0f;		// 炎の半径の減算量
		const int	DIV_RAD_RAND	= 61;	// 炎の半径の剰余算の値
		const int	SUB_RAD_RAND	= 30;	// 炎の半径の減算の値
		const int	DIV_SUBRAD_RAND	= 4;	// 炎の半径の減算量の剰余算の値
		const float	MUL_SUBRAD_RAND	= 1.5f;	// 炎の半径の減算量の減算の値
	}

	// 小爆発
	namespace smallExplosion
	{
		namespace fire
		{
			const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// 爆発の炎のαブレンド
			const D3DXCOLOR COL = D3DXCOLOR(1.0f, 0.38f, 0.23f, 1.0f);	// 爆発の炎の色

			const float	POSGAP		= 5.0f;		// 爆発の炎の位置ずれ量
			const float	MOVE		= 2.0f;		// 爆発の炎の移動量
			const int	SPAWN		= 48;		// 爆発の炎の生成数
			const int	EFF_LIFE	= 18;		// 爆発の炎の寿命
			const int	RAND_LIFE	= 8;		// 爆発の炎のランダム寿命加算量の最大値
			const float	SIZE		= 3.27f;	// 爆発の炎の大きさ
			const float	SUB_SIZE	= -8.5f;	// 爆発の炎の半径の減算量
		}

		namespace smoke
		{
			const CRenderState::EBlend BLEND = CRenderState::BLEND_NORMAL;	// 爆発の煙のαブレンド
			const D3DXCOLOR COL = D3DXCOLOR(0.25f, 0.25f, 0.25f, 1.0f);		// 爆発の煙の色

			const float	POSGAP		= 3.0f;		// 爆発の煙の位置ずれ量
			const float	MOVE		= 1.2f;		// 爆発の煙の移動量
			const int	SPAWN		= 66;		// 爆発の煙の生成数
			const int	EFF_LIFE	= 44;		// 爆発の煙の寿命
			const int	RAND_LIFE	= 12;		// 爆発の煙のランダム寿命加算量の最大値
			const float	SIZE		= 70.0f;	// 爆発の煙の大きさ
			const float	SUB_SIZE	= -1.0f;	// 爆発の煙の半径の減算量
		}
	}

	// 大爆発
	namespace bigExplosion
	{
		namespace fire
		{
			const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// 爆発の炎のαブレンド
			const D3DXCOLOR COL = D3DXCOLOR(1.0f, 0.38f, 0.23f, 1.0f);	// 爆発の炎の色

			const float	POSGAP		= 20.0f;	// 爆発の炎の位置ずれ量
			const float	MOVE		= 3.94f;	// 爆発の炎の移動量
			const int	SPAWN		= 62;		// 爆発の炎の生成数
			const int	EFF_LIFE	= 58;		// 爆発の炎の寿命
			const int	RAND_LIFE	= 12;		// 爆発の炎のランダム寿命加算量の最大値
			const float	SIZE		= 6.57f;	// 爆発の炎の大きさ
			const float	SUB_SIZE	= -14.0f;	// 爆発の炎の半径の減算量
		}

		namespace smoke
		{
			const CRenderState::EBlend BLEND = CRenderState::BLEND_NORMAL;	// 爆発の煙のαブレンド
			const D3DXCOLOR COL = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);		// 爆発の煙の色

			const float	POSGAP		= 10.0f;	// 爆発の煙の位置ずれ量
			const float	MOVE		= 1.4f;		// 爆発の煙の移動量
			const int	SPAWN		= 82;		// 爆発の煙の生成数
			const int	EFF_LIFE	= 146;		// 爆発の煙の寿命
			const int	RAND_LIFE	= 18;		// 爆発の煙のランダム寿命加算量の最大値
			const float	SIZE		= 100.0f;	// 爆発の煙の大きさ
			const float	SUB_SIZE	= -5.5f;	// 爆発の煙の半径の減算量
		}
	}

	// プレイヤーダメージ
	namespace playerDamage
	{
		const CRenderState::EBlend BLEND = CRenderState::BLEND_ADD;	// プレイヤーダメージのαブレンド

		const float	POSGAP		= 12.0f;	// プレイヤーダメージの位置ずれ量
		const float	MOVE_S		= 6.6f;		// プレイヤーダメージの移動量 (小)
		const float	MOVE_M		= 5.4f;		// プレイヤーダメージの移動量 (中)
		const float	MOVE_L		= 4.2f;		// プレイヤーダメージの移動量 (大)
		const int	EFF_SPAWN	= 4;		// プレイヤーダメージの生成数
		const int	RAND_SPAWN	= 6;		// プレイヤーダメージのランダム生成数加算量の最大値
		const int	EFF_LIFE	= 28;		// プレイヤーダメージの寿命
		const int	RAND_LIFE	= 12;		// プレイヤーダメージのランダム寿命加算量の最大値
		const float	SIZE_S		= 32.0f;	// プレイヤーダメージの大きさ (小)
		const float	SIZE_M		= 42.0f;	// プレイヤーダメージの大きさ (中)
		const float	SIZE_L		= 52.0f;	// プレイヤーダメージの大きさ (大)
		const float	SUB_SIZE	= 0.25f;	// プレイヤーダメージの半径の減算量
	}
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(SET_LIFE) == CParticle3D::TYPE_MAX, "ERROR : Type Count Mismatch");

//************************************************************
//	子クラス [CParticle3D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CParticle3D::CParticle3D() : CObject(CObject::LABEL_PARTICLE, CObject::DIM_3D),
	m_pos	(VEC3_ZERO),	// 位置
	m_col	(XCOL_WHITE),	// 色
	m_type	(TYPE_NONE),	// 種類
	m_nLife	(0)				// 寿命
{

}

//============================================================
//	デストラクタ
//============================================================
CParticle3D::~CParticle3D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CParticle3D::Init(void)
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
void CParticle3D::Uninit(void)
{
	// パーティクル3Dオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CParticle3D::Update(const float fDeltaTime)
{
	if (m_nLife > 0)
	{ // 寿命がある場合

		// 寿命を減算
		m_nLife--;
	}
	else
	{ // 寿命がない場合

		// パーティクル3Dオブジェクトの終了
		Uninit();

		// 関数を抜ける
		return;
	}

	switch (m_type)
	{ // 種類ごとの処理
	case TYPE_DAMAGE:

		// ダメージ
		Damage(m_pos, m_col);

		break;

	case TYPE_HEAL:

		// 回復
		Heal(m_pos, m_col);

		break;

	case TYPE_FIRE:

		// 炎
		Fire(m_pos);

		break;

	case TYPE_SMALL_EXPLOSION:

		// 小爆発
		SmallExplosion(m_pos);

		break;

	case TYPE_BIG_EXPLOSION:

		// 大爆発
		BigExplosion(m_pos);

		break;

	case TYPE_PLAYER_DAMAGE:

		// プレイヤーダメージ
		PlayerDamage(m_pos);

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	描画処理
//============================================================
void CParticle3D::Draw(CShader * /*pShader*/)
{

}

//============================================================
//	位置の設定処理
//============================================================
void CParticle3D::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 位置を設定
	m_pos = rPos;
}

//============================================================
//	生成処理
//============================================================
CParticle3D *CParticle3D::Create(const EType type, const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol)
{
	// パーティクル3Dの生成
	CParticle3D *pParticle3D = new CParticle3D;
	if (pParticle3D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// パーティクル3Dの初期化
		if (FAILED(pParticle3D->Init()))
		{ // 初期化に失敗した場合

			// パーティクル3Dの破棄
			SAFE_DELETE(pParticle3D);
			return nullptr;
		}

		// 種類を設定
		pParticle3D->SetType(type);

		// 位置を設定
		pParticle3D->SetVec3Position(rPos);

		// 色を設定
		pParticle3D->SetColor(rCol);

		// 確保したアドレスを返す
		return pParticle3D;
	}
}

//============================================================
//	色の設定処理
//============================================================
void CParticle3D::SetColor(const D3DXCOLOR& rCol)
{
	// 色を設定
	m_col = rCol;
}

//============================================================
//	種類の設定処理
//============================================================
void CParticle3D::SetType(const EType type)
{
	if (type > NONE_IDX && type < TYPE_MAX)
	{ // 種類が正規の場合

		// 種類を設定
		m_type = type;

		// 寿命を設定
		m_nLife = SET_LIFE[(int)type];
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	ダメージ
//============================================================
void CParticle3D::Damage(const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol)
{
	// 変数を宣言
	D3DXVECTOR3 move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3 rot  = VEC3_ZERO;	// 向きの代入用

	if ((m_nLife + 1) % 12 == 0)
	{ // 寿命が12の倍数の場合

		for (int nCntPart = 0; nCntPart < damage::SPAWN; nCntPart++)
		{ // 生成されるエフェクト数分繰り返す

			// ベクトルをランダムに設定
			move.x = sinf(useful::RandomRot());
			move.y = cosf(useful::RandomRot());
			move.z = cosf(useful::RandomRot());

			// ベクトルを正規化
			D3DXVec3Normalize(&move, &move);

			// 移動量を設定
			move.x *= damage::MOVE;
			move.y *= damage::MOVE;
			move.z *= damage::MOVE;

			// 向きを設定
			rot = VEC3_ZERO;

			// エフェクト3Dオブジェクトの生成
			CEffect3D::Create
			( // 引数
				rPos,					// 位置
				damage::SIZE,			// 半径
				CEffect3D::TYPE_NORMAL,	// テクスチャ
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
//	回復
//============================================================
void CParticle3D::Heal(const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol)
{
	// 変数を宣言
	D3DXVECTOR3 vec  = VEC3_ZERO;	// ベクトルの設定用
	D3DXVECTOR3 pos  = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3 rot  = VEC3_ZERO;	// 向きの代入用

	for (int nCntPart = 0; nCntPart < heal::SPAWN; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		vec.x = sinf(useful::RandomRot());
		vec.y = cosf(useful::RandomRot());
		vec.z = cosf(useful::RandomRot());

		// ベクトルを正規化
		D3DXVec3Normalize(&vec, &vec);

		// 位置を設定
		pos = rPos + vec * heal::POSGAP;

		// 移動量を設定
		move = vec * heal::MOVE;

		// 向きを設定
		rot = VEC3_ZERO;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,					// 位置
			heal::SIZE,				// 半径
			CEffect3D::TYPE_HEAL,	// テクスチャ
			heal::EFF_LIFE,			// 寿命
			move,					// 移動量
			rot,					// 向き
			rCol,					// 色
			heal::SUB_SIZE,			// 半径の減算量
			heal::BLEND,			// 加算合成状況
			LABEL_PARTICLE			// オブジェクトラベル
		);
	}
}

//============================================================
//	炎
//============================================================
void CParticle3D::Fire(const D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3	pos  = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3	move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3	rot  = VEC3_ZERO;	// 向きの代入用
	float		fRadius = 0.0f;		// 半径の代入用
	float		fSubRad = 0.0f;		// 半径減算量の代入用

	for (int nCntPart = 0; nCntPart < fire::SPAWN; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// 位置をランダムに設定
		float fRotX = useful::RandomRot();	// ランダム仰角
		float fRotY = useful::RandomRot();	// ランダム方位角
		pos.x = rPos.x + fire::POSGAP * sinf(fRotX) * sinf(fRotY);
		pos.y = rPos.y + fire::POSGAP * cosf(fRotX);
		pos.z = rPos.z + fire::POSGAP * sinf(fRotX) * cosf(fRotY);

		// 移動量を設定
		move.x = fire::MOVE * sinf(fRotX) * sinf(fRotY);
		move.y = fire::MOVE * cosf(fRotX);
		move.z = fire::MOVE * sinf(fRotX) * cosf(fRotY);

		// 向きを設定
		rot.z = useful::RandomRot();

		// 半径を設定
		fRadius = fire::INIT_RAD + (float)(rand() % fire::DIV_RAD_RAND - fire::SUB_RAD_RAND);

		// 半径減算量を設定
		fSubRad = fire::INIT_SUBRAD + rand() % fire::DIV_SUBRAD_RAND * fire::MUL_SUBRAD_RAND;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,					// 位置
			fRadius,				// 半径
			CEffect3D::TYPE_FIRE,	// テクスチャ
			fire::EFF_LIFE,			// 寿命
			move,					// 移動量
			rot,					// 向き
			fire::COL,				// 色
			fSubRad,				// 半径の減算量
			fire::BLEND,			// 加算合成状況
			LABEL_PARTICLE			// オブジェクトラベル
		);
	}
}

//============================================================
//	小爆発
//============================================================
void CParticle3D::SmallExplosion(const D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 vec  = VEC3_ZERO;	// ベクトルの設定用
	D3DXVECTOR3 pos  = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3 rot  = VEC3_ZERO;	// 向きの代入用
	int nLife = 0;	// 寿命の代入用

	for (int nCntPart = 0; nCntPart < smallExplosion::smoke::SPAWN; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		vec.x = sinf(useful::RandomRot());
		vec.y = cosf(useful::RandomRot());
		vec.z = cosf(useful::RandomRot());

		// ベクトルを正規化
		D3DXVec3Normalize(&vec, &vec);

		// 位置を設定
		pos = rPos + vec * smallExplosion::smoke::POSGAP;

		// 移動量を設定
		move = vec * smallExplosion::smoke::MOVE;

		// 向きを設定
		rot.x = 0.0f;
		rot.y = 0.0f;
		rot.z = useful::RandomRot();

		// 寿命を設定
		nLife = (rand() % smallExplosion::smoke::RAND_LIFE) + smallExplosion::smoke::EFF_LIFE;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,								// 位置
			smallExplosion::smoke::SIZE,		// 半径
			CEffect3D::TYPE_SMOKE,				// テクスチャ
			nLife,								// 寿命
			move,								// 移動量
			rot,								// 向き
			smallExplosion::smoke::COL,			// 色
			smallExplosion::smoke::SUB_SIZE,	// 半径の減算量
			smallExplosion::smoke::BLEND,		// 加算合成状況
			LABEL_PARTICLE						// オブジェクトラベル
		);
	}

	for (int nCntPart = 0; nCntPart < smallExplosion::fire::SPAWN; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		vec.x = sinf(useful::RandomRot());
		vec.y = cosf(useful::RandomRot());
		vec.z = cosf(useful::RandomRot());

		// ベクトルを正規化
		D3DXVec3Normalize(&vec, &vec);

		// 位置を設定
		pos = rPos + vec * smallExplosion::fire::POSGAP;

		// 移動量を設定
		move = vec * smallExplosion::fire::MOVE;

		// 向きを設定
		rot = VEC3_ZERO;

		// 寿命を設定
		nLife = (rand() % smallExplosion::fire::RAND_LIFE) + smallExplosion::fire::EFF_LIFE;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,							// 位置
			smallExplosion::fire::SIZE,		// 半径
			CEffect3D::TYPE_NORMAL,			// テクスチャ
			nLife,							// 寿命
			move,							// 移動量
			rot,							// 向き
			smallExplosion::fire::COL,		// 色
			smallExplosion::fire::SUB_SIZE,	// 半径の減算量
			smallExplosion::fire::BLEND,	// 加算合成状況
			LABEL_PARTICLE					// オブジェクトラベル
		);
	}
}

//============================================================
//	大爆発
//============================================================
void CParticle3D::BigExplosion(const D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 vec  = VEC3_ZERO;	// ベクトルの設定用
	D3DXVECTOR3 pos  = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3 rot  = VEC3_ZERO;	// 向きの代入用
	int nLife = 0;	// 寿命の代入用

	for (int nCntPart = 0; nCntPart < bigExplosion::smoke::SPAWN; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		vec.x = sinf(useful::RandomRot());
		vec.y = cosf(useful::RandomRot());
		vec.z = cosf(useful::RandomRot());

		// ベクトルを正規化
		D3DXVec3Normalize(&vec, &vec);

		// 位置を設定
		pos = rPos + vec * bigExplosion::smoke::POSGAP;

		// 移動量を設定
		move = vec * bigExplosion::smoke::MOVE;

		// 向きを設定
		rot.x = 0.0f;
		rot.y = 0.0f;
		rot.z = useful::RandomRot();

		// 寿命を設定
		nLife = (rand() % bigExplosion::smoke::RAND_LIFE) + bigExplosion::smoke::EFF_LIFE;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,							// 位置
			bigExplosion::smoke::SIZE,		// 半径
			CEffect3D::TYPE_SMOKE,			// テクスチャ
			nLife,							// 寿命
			move,							// 移動量
			rot,							// 向き
			bigExplosion::smoke::COL,		// 色
			bigExplosion::smoke::SUB_SIZE,	// 半径の減算量
			bigExplosion::smoke::BLEND,		// 加算合成状況
			LABEL_PARTICLE					// オブジェクトラベル
		);
	}

	for (int nCntPart = 0; nCntPart < bigExplosion::fire::SPAWN; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		vec.x = sinf(useful::RandomRot());
		vec.y = cosf(useful::RandomRot());
		vec.z = cosf(useful::RandomRot());

		// ベクトルを正規化
		D3DXVec3Normalize(&vec, &vec);

		// 位置を設定
		pos = rPos + vec * bigExplosion::fire::POSGAP;

		// 移動量を設定
		move = vec * bigExplosion::fire::MOVE;

		// 向きを設定
		rot = VEC3_ZERO;

		// 寿命を設定
		nLife = (rand() % bigExplosion::fire::RAND_LIFE) + bigExplosion::fire::EFF_LIFE;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,							// 位置
			bigExplosion::fire::SIZE,		// 半径
			CEffect3D::TYPE_NORMAL,			// テクスチャ
			nLife,							// 寿命
			move,							// 移動量
			rot,							// 向き
			bigExplosion::fire::COL,		// 色
			bigExplosion::fire::SUB_SIZE,	// 半径の減算量
			bigExplosion::fire::BLEND,		// 加算合成状況
			LABEL_PARTICLE					// オブジェクトラベル
		);
	}
}

//============================================================
//	プレイヤーダメージ
//============================================================
void CParticle3D::PlayerDamage(const D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 vec  = VEC3_ZERO;	// ベクトルの設定用
	D3DXVECTOR3 pos  = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 move = VEC3_ZERO;	// 移動量の代入用
	D3DXVECTOR3 rot  = VEC3_ZERO;	// 向きの代入用
	D3DXCOLOR   col  = XCOL_WHITE;	// 色の代入用
	int nSpawn = 0;	// 生成数の代入用
	int nLife = 0;	// 寿命の代入用

	// 生成数を設定
	nSpawn = (rand() % playerDamage::EFF_SPAWN) + playerDamage::RAND_SPAWN;

	for (int nCntPart = 0; nCntPart < nSpawn; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		vec.x = sinf(useful::RandomRot());
		vec.y = cosf(useful::RandomRot());
		vec.z = cosf(useful::RandomRot());

		// ベクトルを正規化
		D3DXVec3Normalize(&vec, &vec);

		// 位置を設定
		pos = rPos + vec * playerDamage::POSGAP;

		// 移動量を設定
		move = vec * playerDamage::MOVE_S;

		// 向きを設定
		rot.x = 0.0f;
		rot.y = 0.0f;
		rot.z = useful::RandomRot();

		// 色を設定
		col.r = (float)(rand() % 80 + 20) / 100.0f;
		col.g = (float)(rand() % 20 + 80) / 100.0f;
		col.b = (float)(rand() % 80 + 20) / 100.0f;
		col.a = 1.0f;

		// 寿命を設定
		nLife = (rand() % playerDamage::RAND_LIFE) + playerDamage::EFF_LIFE;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,						// 位置
			playerDamage::SIZE_S,		// 半径
			CEffect3D::TYPE_PIECE_S,	// テクスチャ
			nLife,						// 寿命
			move,						// 移動量
			rot,						// 向き
			col,						// 色
			playerDamage::SUB_SIZE,		// 半径の減算量
			playerDamage::BLEND,		// 加算合成状況
			LABEL_PARTICLE				// オブジェクトラベル
		);
	}

	// 生成数を設定
	nSpawn = (rand() % playerDamage::EFF_SPAWN) + playerDamage::RAND_SPAWN;

	for (int nCntPart = 0; nCntPart < nSpawn; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		vec.x = sinf(useful::RandomRot());
		vec.y = cosf(useful::RandomRot());
		vec.z = cosf(useful::RandomRot());

		// ベクトルを正規化
		D3DXVec3Normalize(&vec, &vec);

		// 位置を設定
		pos = rPos + vec * playerDamage::POSGAP;

		// 移動量を設定
		move = vec * playerDamage::MOVE_M;

		// 向きを設定
		rot.x = 0.0f;
		rot.y = 0.0f;
		rot.z = useful::RandomRot();

		// 色を設定
		col.r = (float)(rand() % 20 + 80) / 100.0f;
		col.g = (float)(rand() % 80 + 20) / 100.0f;
		col.b = (float)(rand() % 80 + 20) / 100.0f;
		col.a = 1.0f;

		// 寿命を設定
		nLife = (rand() % playerDamage::RAND_LIFE) + playerDamage::EFF_LIFE;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,						// 位置
			playerDamage::SIZE_M,		// 半径
			CEffect3D::TYPE_PIECE_M,	// テクスチャ
			nLife,						// 寿命
			move,						// 移動量
			rot,						// 向き
			col,						// 色
			playerDamage::SUB_SIZE,		// 半径の減算量
			playerDamage::BLEND,		// 加算合成状況
			LABEL_PARTICLE				// オブジェクトラベル
		);
	}

	// 生成数を設定
	nSpawn = (rand() % playerDamage::EFF_SPAWN) + playerDamage::RAND_SPAWN;

	for (int nCntPart = 0; nCntPart < nSpawn; nCntPart++)
	{ // 生成されるエフェクト数分繰り返す

		// ベクトルをランダムに設定
		vec.x = sinf(useful::RandomRot());
		vec.y = cosf(useful::RandomRot());
		vec.z = cosf(useful::RandomRot());

		// ベクトルを正規化
		D3DXVec3Normalize(&vec, &vec);

		// 位置を設定
		pos = rPos + vec * playerDamage::POSGAP;

		// 移動量を設定
		move = vec * playerDamage::MOVE_L;

		// 向きを設定
		rot.x = 0.0f;
		rot.y = 0.0f;
		rot.z = useful::RandomRot();

		// 色を設定
		col.r = (float)(rand() % 80 + 20) / 100.0f;
		col.g = (float)(rand() % 80 + 20) / 100.0f;
		col.b = (float)(rand() % 20 + 80) / 100.0f;
		col.a = 1.0f;

		// 寿命を設定
		nLife = (rand() % playerDamage::RAND_LIFE) + playerDamage::EFF_LIFE;

		// エフェクト3Dオブジェクトの生成
		CEffect3D::Create
		( // 引数
			pos,						// 位置
			playerDamage::SIZE_L,		// 半径
			CEffect3D::TYPE_PIECE_L,	// テクスチャ
			nLife,						// 寿命
			move,						// 移動量
			rot,						// 向き
			col,						// 色
			playerDamage::SUB_SIZE,		// 半径の減算量
			playerDamage::BLEND,		// 加算合成状況
			LABEL_PARTICLE				// オブジェクトラベル
		);
	}
}
