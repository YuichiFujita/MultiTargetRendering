//============================================================
//
//	スクロール2D処理 [scroll2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "scroll2D.h"

//************************************************************
//	子クラス [CScroll2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CScroll2D::CScroll2D(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject2D(label, dimension, nPriority),
	m_fTexU		(0.0f),	// テクスチャ横座標の開始位置
	m_fTexV		(0.0f),	// テクスチャ縦座標の開始位置
	m_fMoveU	(0.0f),	// テクスチャ横座標の移動量
	m_fMoveV	(0.0f)	// テクスチャ縦座標の移動量
{

}

//============================================================
//	デストラクタ
//============================================================
CScroll2D::~CScroll2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CScroll2D::Init(void)
{
	// メンバ変数を初期化
	m_fTexU  = 0.0f;	// テクスチャ横座標の開始位置
	m_fTexV  = 0.0f;	// テクスチャ縦座標の開始位置
	m_fMoveU = 0.0f;	// テクスチャ横座標の移動量
	m_fMoveV = 0.0f;	// テクスチャ縦座標の移動量

	// オブジェクト2Dの初期化
	if (FAILED(CObject2D::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CScroll2D::Uninit(void)
{
	// オブジェクト2Dの終了
	CObject2D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CScroll2D::Update(const float fDeltaTime)
{
	// スクロールを加算
	m_fTexU += m_fMoveU;
	m_fTexV += m_fMoveV;

	if (m_fTexU > 1.0f)
	{ // 1.0fより大きくなった場合

		// 開始地点を補正
		m_fTexU -= 1.0f;
	}

	if (m_fTexV > 1.0f)
	{ // 1.0fより大きくなった場合

		// 開始地点を補正
		m_fTexV -= 1.0f;
	}

	// オブジェクト2Dの更新
	CObject2D::Update(fDeltaTime);

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	描画処理
//============================================================
void CScroll2D::Draw(CShader *pShader)
{
	// オブジェクト2Dの描画
	CObject2D::Draw(pShader);
}

//============================================================
//	位置の設定処理
//============================================================
void CScroll2D::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 位置の設定
	CObject2D::SetVec3Position(rPos);

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	向きの設定処理
//============================================================
void CScroll2D::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 向きの設定
	CObject2D::SetVec3Rotation(rRot);

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	大きさの設定処理
//============================================================
void CScroll2D::SetVec3Sizing(const D3DXVECTOR3& rSize)
{
	// 大きさの設定
	CObject2D::SetVec3Sizing(rSize);

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	生成処理
//============================================================
CScroll2D *CScroll2D::Create
(
	const float fMoveU,
	const float fMoveV,
	const D3DXVECTOR3& rPos,
	const D3DXVECTOR3& rSize,
	const D3DXVECTOR3& rRot,
	const D3DXCOLOR& rCol
)
{
	// スクロール2Dの生成
	CScroll2D *pScroll2D = new CScroll2D;
	if (pScroll2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// スクロール2Dの初期化
		if (FAILED(pScroll2D->Init()))
		{ // 初期化に失敗した場合

			// スクロール2Dの破棄
			SAFE_DELETE(pScroll2D);
			return nullptr;
		}

		// 位置を設定
		pScroll2D->SetVec3Position(rPos);

		// 向きを設定
		pScroll2D->SetVec3Rotation(rRot);

		// 大きさを設定
		pScroll2D->SetVec3Sizing(rSize);

		// 色を設定
		pScroll2D->SetColor(rCol);

		// 横座標の移動量を設定
		pScroll2D->SetMoveU(fMoveU);

		// 縦座標の移動量を設定
		pScroll2D->SetMoveV(fMoveV);

		// 確保したアドレスを返す
		return pScroll2D;
	}
}

//============================================================
//	色の設定処理
//============================================================
void CScroll2D::SetColor(const D3DXCOLOR& rCol)
{
	// 色の設定
	CObject2D::SetColor(rCol);

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	横座標の開始位置設定処理
//============================================================
void CScroll2D::SetTexU(const float fTexU)
{
	// 引数の横座標の開始位置を代入
	m_fTexU = fTexU;

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	縦座標の開始位置設定処理
//============================================================
void CScroll2D::SetTexV(const float fTexV)
{
	// 引数の縦座標の開始位置を代入
	m_fTexV = fTexV;

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	横座標の移動量設定処理
//============================================================
void CScroll2D::SetMoveU(const float fMoveU)
{
	// 引数の横座標の移動量を代入
	m_fMoveU = fMoveU;

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	縦座標の移動量設定処理
//============================================================
void CScroll2D::SetMoveV(const float fMoveV)
{
	// 引数の縦座標の移動量を代入
	m_fMoveV = fMoveV;

	// スクロールのテクスチャ座標の設定
	CObject2D::SetScrollTex(m_fTexU, m_fTexV);
}
