//============================================================
//
//	スクロールメッシュフィールド処理 [scrollMeshField.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "scrollMeshField.h"

//************************************************************
//	子クラス [CScrollMeshField] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CScrollMeshField::CScrollMeshField(const CObject::ELabel label, const EDim dimension, const int nPriority) : CObjectMeshField(label, dimension, nPriority),
	m_fTexU		(0.0f),	// テクスチャ横座標の開始位置
	m_fTexV		(0.0f),	// テクスチャ縦座標の開始位置
	m_fMoveU	(0.0f),	// テクスチャ横座標の移動量
	m_fMoveV	(0.0f)	// テクスチャ縦座標の移動量
{

}

//============================================================
//	デストラクタ
//============================================================
CScrollMeshField::~CScrollMeshField()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CScrollMeshField::Init(void)
{
	// メンバ変数を初期化
	m_fTexU  = 0.0f;	// テクスチャ横座標の開始位置
	m_fTexV  = 0.0f;	// テクスチャ縦座標の開始位置
	m_fMoveU = 0.0f;	// テクスチャ横座標の移動量
	m_fMoveV = 0.0f;	// テクスチャ縦座標の移動量

	// メッシュフィールドの初期化
	if (FAILED(CObjectMeshField::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CScrollMeshField::Uninit(void)
{
	// メッシュフィールドの終了
	CObjectMeshField::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CScrollMeshField::Update(const float fDeltaTime)
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

	// メッシュフィールドの更新
	CObjectMeshField::Update(fDeltaTime);

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	描画処理
//============================================================
void CScrollMeshField::Draw(CShader *pShader)
{
	// メッシュフィールドの描画
	CObjectMeshField::Draw(pShader);
}

//============================================================
//	位置の設定処理
//============================================================
void CScrollMeshField::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 位置の設定
	CObjectMeshField::SetVec3Position(rPos);

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	向きの設定処理
//============================================================
void CScrollMeshField::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 向きの設定
	CObjectMeshField::SetVec3Rotation(rRot);

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	大きさの設定処理
//============================================================
void CScrollMeshField::SetVec2Sizing(const D3DXVECTOR2& rSize)
{
	// 大きさの設定
	CObjectMeshField::SetVec2Sizing(rSize);

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	生成処理
//============================================================
CScrollMeshField *CScrollMeshField::Create
(
	const float fMoveU,			// 横座標の移動量
	const float fMoveV,			// 縦座標の移動量
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR2& rSize,	// 大きさ
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart		// 分割数
)
{
	// スクロールメッシュフィールドの生成
	CScrollMeshField *pScrollMeshField = new CScrollMeshField;
	if (pScrollMeshField == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// スクロールメッシュフィールドの初期化
		if (FAILED(pScrollMeshField->Init()))
		{ // 初期化に失敗した場合

			// スクロールメッシュフィールドの破棄
			SAFE_DELETE(pScrollMeshField);
			return nullptr;
		}

		// 位置を設定
		pScrollMeshField->SetVec3Position(rPos);

		// 向きを設定
		pScrollMeshField->SetVec3Rotation(rRot);

		// 大きさを設定
		pScrollMeshField->SetVec2Sizing(rSize);

		// 色を設定
		pScrollMeshField->SetColor(rCol);

		// 横座標の移動量を設定
		pScrollMeshField->SetMoveU(fMoveU);

		// 縦座標の移動量を設定
		pScrollMeshField->SetMoveV(fMoveV);

		// 分割数を設定
		if (FAILED(pScrollMeshField->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// スクロールメッシュフィールドの破棄
			SAFE_DELETE(pScrollMeshField);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pScrollMeshField;
	}
}

//============================================================
//	地形の設定処理
//============================================================
void CScrollMeshField::SetTerrain(const POSGRID2& rPart, D3DXVECTOR3 *pPosGap)
{
	// 地形の設定
	CObjectMeshField::SetTerrain(rPart, pPosGap);

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	色の設定処理
//============================================================
void CScrollMeshField::SetColor(const D3DXCOLOR& rCol)
{
	// 色の設定
	CObjectMeshField::SetColor(rCol);

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	分割数の設定処理
//============================================================
HRESULT CScrollMeshField::SetPattern(const POSGRID2& rPart)
{
	// 分割数の設定
	if (FAILED(CObjectMeshField::SetPattern(rPart)))
	{ // 分割数の設定に失敗した場合

		// 失敗を返す
		return E_FAIL;
	}

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);

	// 成功を返す
	return S_OK;
}

//============================================================
//	横座標の開始位置設定処理
//============================================================
void CScrollMeshField::SetTexU(const float fTexU)
{
	// 引数の横座標の開始位置を代入
	m_fTexU = fTexU;

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	縦座標の開始位置設定処理
//============================================================
void CScrollMeshField::SetTexV(const float fTexV)
{
	// 引数の縦座標の開始位置を代入
	m_fTexV = fTexV;

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	横座標の移動量設定処理
//============================================================
void CScrollMeshField::SetMoveU(const float fMoveU)
{
	// 引数の横座標の移動量を代入
	m_fMoveU = fMoveU;

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	横座標の移動量取得処理
//============================================================
float CScrollMeshField::GetMoveU(void) const
{
	// テクスチャ横座標の移動量を返す
	return m_fMoveU;
}

//============================================================
//	縦座標の移動量設定処理
//============================================================
void CScrollMeshField::SetMoveV(const float fMoveV)
{
	// 引数の縦座標の移動量を代入
	m_fMoveV = fMoveV;

	// スクロールのテクスチャ座標の設定
	CObjectMeshField::SetScrollTex(m_fTexU, m_fTexV);
}

//============================================================
//	縦座標の移動量取得処理
//============================================================
float CScrollMeshField::GetMoveV(void) const
{
	// テクスチャ縦座標の移動量を返す
	return m_fMoveV;
}
