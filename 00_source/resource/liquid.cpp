//============================================================
//
//	液体処理 [liquid.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "liquid.h"
#include "manager.h"
#include "renderer.h"
#include "scrollMeshField.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[][CLiquid::LIQUID_MAX] =	// テクスチャファイル
	{
		{ // マグマテクスチャ
			"data\\TEXTURE\\lava000.png",	// マグマ (下)
			"data\\TEXTURE\\lava001.png",	// マグマ (上)
		},
	};

	const int PRIORITY = 0;	// 液体の優先順位
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CLiquid::TYPE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CLiquid] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CLiquid::CLiquid() : CObject(CObject::LABEL_LIQUID, CObject::DIM_3D, PRIORITY),
	m_type			(TYPE_LAVA),	// 種類
	m_fMaxUp		(0.0f),			// 波の最高上昇量
	m_fSinRot		(0.0f),			// 波打ち向き
	m_fAddSinRot	(0.0f),			// 波打ち向き加算量
	m_fAddVtxRot	(0.0f)			// 隣波の向き加算量
{
	// メンバ変数をクリア
	memset(&m_apLiquid[0], 0, sizeof(m_apLiquid));	// 液体の情報
}

//============================================================
//	デストラクタ
//============================================================
CLiquid::~CLiquid()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CLiquid::Init(void)
{
	// メンバ変数を初期化
	memset(&m_apLiquid[0], 0, sizeof(m_apLiquid));	// 液体の情報
	m_type			= TYPE_LAVA;	// 種類
	m_fMaxUp		= 0.0f;			// 波の最高上昇量
	m_fSinRot		= 0.0f;			// 波打ち向き
	m_fAddSinRot	= 0.0f;			// 波打ち向き加算量
	m_fAddVtxRot	= 0.0f;			// 隣波の向き加算量

	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 液体の生成
		m_apLiquid[nCntLiquid] = CScrollMeshField::Create
		( // 引数
			0.0f,			// 横座標の移動量
			0.0f,			// 縦座標の移動量
			VEC3_ZERO,		// 位置
			VEC3_ZERO,		// 向き
			VEC2_ZERO,		// 大きさ
			XCOL_WHITE,		// 色
			GRID2_ONE		// 分割数
		);
		if (m_apLiquid[nCntLiquid] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録・割当
		m_apLiquid[nCntLiquid]->BindTexture(TEXTURE_FILE[m_type][nCntLiquid]);

		// 自動更新・自動描画をOFFにする
		m_apLiquid[nCntLiquid]->SetEnableUpdate(false);
		m_apLiquid[nCntLiquid]->SetEnableDraw(false);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CLiquid::Uninit(void)
{
	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 液体の終了
		SAFE_UNINIT(m_apLiquid[nCntLiquid]);
	}

	// 自身のオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CLiquid::Update(const float fDeltaTime)
{
	// 変数を宣言
	POSGRID2 part = GetPattern();	// 分割数
	D3DXVECTOR3 pos;	// 頂点座標取得用
	int nLine;			// 頂点の行
	float fSinRot;		// サインカーブに使用する角度

	// メッシュの頂点座標の更新
	for (int nCntVtx = 0; nCntVtx < (part.x + 1) * (part.y + 1); nCntVtx++)
	{ // 使用頂点数分繰り返す

		for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
		{ // 液体の最大数分繰り返す

			// 頂点座標を取得
			pos = m_apLiquid[nCntLiquid]->GetMeshVertexPosition(nCntVtx);

			// 波打ち用の角度を求める
			nLine = nCntVtx / (part.x + 1);				// 頂点の行
			fSinRot = m_fSinRot + nLine * m_fAddVtxRot;	// サインカーブに使用する角度

			// サインカーブ用の向きを補正
			useful::NormalizeRot(fSinRot);

			// 頂点座標を設定
			m_apLiquid[nCntLiquid]->SetMeshVertexPosition(nCntVtx, D3DXVECTOR3(pos.x, (sinf(fSinRot) * m_fMaxUp) + m_fMaxUp, pos.z));
		}
	}

	// 波打ち向きを加算
	m_fSinRot += m_fAddSinRot;

	// 向きを補正
	useful::NormalizeRot(m_fSinRot);

	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 液体の更新
		m_apLiquid[nCntLiquid]->Update(fDeltaTime);
	}
}

//============================================================
//	描画処理
//============================================================
void CLiquid::Draw(CShader *pShader)
{
	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 液体の描画
		m_apLiquid[nCntLiquid]->Draw(pShader);
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CLiquid::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 液体の位置を設定
	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 引数の位置を設定
		m_apLiquid[nCntLiquid]->SetVec3Position(rPos);
	}
}

//============================================================
//	向きの設定処理
//============================================================
void CLiquid::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 液体の向きを設定
	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 引数の向きを設定
		m_apLiquid[nCntLiquid]->SetVec3Rotation(rRot);
	}
}

//============================================================
//	大きさの設定処理
//============================================================
void CLiquid::SetVec2Sizing(const D3DXVECTOR2& rSize)
{
	// 液体の大きさを設定
	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 引数の大きさを設定
		m_apLiquid[nCntLiquid]->SetVec2Sizing(rSize);
	}
}

//============================================================
//	生成処理
//============================================================
CLiquid *CLiquid::Create
(
	const EType type,			// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR2& rSize,	// 大きさ
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart,		// 分割数
	const STexMove& rTexMove,	// テクスチャ移動量
	const float fMaxUp,			// 波の最高上昇量
	const float fAddSinRot,		// 波打ち向き加算量
	const float fAddVtxRot		// 隣波の向き加算量
)
{
	// 液体の生成
	CLiquid *pLiquid = new CLiquid;
	if (pLiquid == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 液体の初期化
		if (FAILED(pLiquid->Init()))
		{ // 初期化に失敗した場合

			// 液体の破棄
			SAFE_DELETE(pLiquid);
			return nullptr;
		}

		// 種類を設定
		pLiquid->SetType(type);

		// 位置を設定
		pLiquid->SetVec3Position(rPos);

		// 向きを設定
		pLiquid->SetVec3Rotation(rRot);

		// 大きさを設定
		pLiquid->SetVec2Sizing(rSize);

		// 色を設定
		pLiquid->SetColor(rCol);

		// テクスチャ移動量を設定
		pLiquid->SetTexMove(rTexMove);

		// 波の最高上昇量を設定
		pLiquid->SetMaxUp(fMaxUp);

		// 波打ち向き加算量を設定
		pLiquid->SetAddSinRot(fAddSinRot);

		// 隣波の向き加算量を設定
		pLiquid->SetAddVtxRot(fAddVtxRot);

		// 分割数を設定
		if (FAILED(pLiquid->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// 液体の破棄
			SAFE_DELETE(pLiquid);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pLiquid;
	}
}

//============================================================
//	色の設定処理
//============================================================
void CLiquid::SetColor(const D3DXCOLOR& rCol)
{
	// 液体の色を設定
	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 引数の色を設定
		m_apLiquid[nCntLiquid]->SetColor(rCol);
	}
}

//============================================================
//	分割数の設定処理
//============================================================
HRESULT CLiquid::SetPattern(const POSGRID2& rPart)
{
	// 液体の分割数を設定
	for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
	{ // 液体の最大数分繰り返す

		// 引数の分割数を設定
		if (FAILED(m_apLiquid[nCntLiquid]->SetPattern(rPart)))
		{ // 設定に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	種類の設定処理
//============================================================
void CLiquid::SetType(const EType type)
{
	if (type > NONE_IDX && type < TYPE_MAX)
	{ // 種類が範囲内の場合

		// 引数の種類を設定
		m_type = type;

		for (int nCntLiquid = 0; nCntLiquid < LIQUID_MAX; nCntLiquid++)
		{ // 液体の最大数分繰り返す

			// テクスチャを登録・割当
			m_apLiquid[nCntLiquid]->BindTexture(TEXTURE_FILE[(int)m_type][nCntLiquid]);
		}
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	テクスチャ移動量の設定処理
//============================================================
void CLiquid::SetTexMove(const STexMove texMove)
{
	// 下液体のテクスチャ移動量を設定
	m_apLiquid[LIQUID_LOW]->SetMoveU(texMove.texMoveLow.x);
	m_apLiquid[LIQUID_LOW]->SetMoveV(texMove.texMoveLow.y);

	// 上液体のテクスチャ移動量を設定
	m_apLiquid[LIQUID_HIGH]->SetMoveU(texMove.texMoveHigh.x);
	m_apLiquid[LIQUID_HIGH]->SetMoveV(texMove.texMoveHigh.y);
}

//============================================================
//	波の最高上昇量の設定処理
//============================================================
void CLiquid::SetMaxUp(const float fMaxUp)
{
	// 引数の波の最高上昇量を設定
	m_fMaxUp = fMaxUp;
}

//============================================================
//	波打ち向き加算量の設定処理
//============================================================
void CLiquid::SetAddSinRot(const float fAddSinRot)
{
	// 引数の波打ち向き加算量を設定
	m_fAddSinRot = fAddSinRot;
}

//============================================================
//	隣波の向き加算量の設定処理
//============================================================
void CLiquid::SetAddVtxRot(const float fAddVtxRot)
{
	// 引数の隣波の向き加算量を設定
	m_fAddVtxRot = fAddVtxRot;
}

//============================================================
//	テクスチャ移動量取得処理
//============================================================
CLiquid::STexMove CLiquid::GetTexMove(void) const
{
	// 変数を宣言
	STexMove texMove;	// テクスチャ移動量

	// 下液体のテクスチャ移動量を設定
	texMove.texMoveLow.x = m_apLiquid[LIQUID_LOW]->GetMoveU();
	texMove.texMoveLow.y = m_apLiquid[LIQUID_LOW]->GetMoveV();

	// 上液体のテクスチャ移動量を設定
	texMove.texMoveHigh.x = m_apLiquid[LIQUID_HIGH]->GetMoveU();
	texMove.texMoveHigh.y = m_apLiquid[LIQUID_HIGH]->GetMoveV();

	// テクスチャ移動量を返す
	return texMove;
}

//============================================================
//	メッシュフィールド取得処理
//============================================================
CScrollMeshField *CLiquid::GetMeshField(const int nID) const
{
	if (nID > NONE_IDX && nID < LIQUID_MAX)
	{ // インデックスが範囲内の場合

		// メッシュフィールドを返す
		return m_apLiquid[nID];
	}
	else { assert(false); return nullptr; }	// 範囲外
}
