//============================================================
//
//	オブジェクトメッシュチューブ処理 [objectMeshTube.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshTube.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "objectMeshCircle.h"
#include "objectMeshCylinder.h"
#include "renderState.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const POSGRID2 MIN_PART		= POSGRID2(1, 2);	// 分割数の最小値
	const POSGRID2 MIN_TEXPART	= GRID2_ONE;		// テクスチャ分割数の最小値
}

//************************************************************
//	子クラス [CObjectMeshTube] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectMeshTube::CObjectMeshTube(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_pCylinder	(nullptr),		// 筒の情報
	m_part		(GRID2_ZERO),	// 分割数
	m_texPart	(GRID2_ZERO)	// テクスチャ分割数
{
	// メンバ変数をクリア
	memset(&m_apCover[0], 0, sizeof(m_apCover));	// 蓋の情報
}

//============================================================
//	デストラクタ
//============================================================
CObjectMeshTube::~CObjectMeshTube()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectMeshTube::Init(void)
{
	// メンバ変数を初期化
	memset(&m_apCover[0], 0, sizeof(m_apCover));	// 蓋の情報
	m_pCylinder	= nullptr;		// 筒の情報
	m_part		= MIN_PART;		// 分割数
	m_texPart	= MIN_TEXPART;	// テクスチャ分割数

	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		// メッシュサークルの生成
		m_apCover[i] = CObjectMeshCircle::Create
		( // 引数
			VEC3_ZERO,		// 位置
			VEC3_ZERO,		// 向き
			XCOL_WHITE,		// 色
			MIN_PART,		// 分割数
			0.0f			// 半径
		);

		// 自動更新・自動描画をOFFにする
		m_apCover[i]->SetEnableUpdate(false);
		m_apCover[i]->SetEnableDraw(false);
	}

	// メッシュシリンダーの生成
	m_pCylinder = CObjectMeshCylinder::Create
	( // 引数
		VEC3_ZERO,		// 位置
		VEC3_ZERO,		// 向き
		XCOL_WHITE,		// 色
		MIN_PART,		// 分割数
		MIN_TEXPART,	// テクスチャ分割数
		0.0f,			// 半径
		0.0f			// 縦幅
	);

	// 自動更新・自動描画をOFFにする
	m_pCylinder->SetEnableUpdate(false);
	m_pCylinder->SetEnableDraw(false);

	// 分割数を設定
	if (FAILED(SetPattern(MIN_PART)))
	{ // 分割数の設定に失敗した場合

		// 失敗を返す
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CObjectMeshTube::Uninit(void)
{
	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		// メッシュサークルの終了
		SAFE_UNINIT(m_apCover[i]);
	}

	// メッシュシリンダーの終了
	SAFE_UNINIT(m_pCylinder);

	// オブジェクトメッシュチューブを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectMeshTube::Update(const float fDeltaTime)
{
	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		// メッシュサークルの更新
		m_apCover[i]->Update(fDeltaTime);
	}

	// メッシュシリンダーの更新
	m_pCylinder->Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CObjectMeshTube::Draw(CShader *pShader)
{
	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		// メッシュサークルの描画
		m_apCover[i]->Draw(pShader);
	}

	// メッシュシリンダーの描画
	m_pCylinder->Draw(pShader);
}

//============================================================
//	優先順位の設定処理
//============================================================
void CObjectMeshTube::SetPriority(const int nPriority)
{
	// 引数の優先順位を設定
	CObject::SetPriority(nPriority);		// 自身
	m_pCylinder->SetPriority(nPriority);	// 筒
	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		m_apCover[i]->SetPriority(nPriority);	// 蓋
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CObjectMeshTube::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pCylinder->SetVec3Position(rPos);

	// 相対位置を設定
	SetPositionRelative();
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectMeshTube::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_pCylinder->SetVec3Rotation(rRot);

	// 相対位置を設定
	SetPositionRelative();
}

//============================================================
//	生成処理
//============================================================
CObjectMeshTube *CObjectMeshTube::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart,		// 分割数
	const POSGRID2& rTexPart,	// テクスチャ分割数
	const float fRadius,		// 半径
	const float fHeight			// 縦幅
)
{
	// オブジェクトメッシュチューブの生成
	CObjectMeshTube *pMeshTube = new CObjectMeshTube;
	if (pMeshTube == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクトメッシュチューブの初期化
		if (FAILED(pMeshTube->Init()))
		{ // 初期化に失敗した場合

			// オブジェクトメッシュチューブの破棄
			SAFE_DELETE(pMeshTube);
			return nullptr;
		}

		// 位置を設定
		pMeshTube->SetVec3Position(rPos);

		// 向きを設定
		pMeshTube->SetVec3Rotation(rRot);

		// 色を設定
		pMeshTube->SetColor(rCol);

		// 半径を設定
		pMeshTube->SetRadius(fRadius);

		// 縦幅を設定
		pMeshTube->SetHeight(fHeight);

		// 分割数を設定
		if (FAILED(pMeshTube->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// オブジェクトメッシュチューブの破棄
			SAFE_DELETE(pMeshTube);
			return nullptr;
		}

		// テクスチャ分割数を設定
		pMeshTube->SetTexPattern(rTexPart);

		// 確保したアドレスを返す
		return pMeshTube;
	}
}

//============================================================
//	レンダーステート情報の設定処理
//============================================================
void CObjectMeshTube::SetRenderState(CRenderState renderState)
{
	CRenderState *pTempRenderState = nullptr;	// レンダーステート情報

	// 引数のレンダーステートを設定
	pTempRenderState = m_pCylinder->GetRenderState();
	*pTempRenderState = renderState;

	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		// 引数のレンダーステートを設定
		pTempRenderState = m_apCover[i]->GetRenderState();
		*pTempRenderState = renderState;
	}
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObjectMeshTube::BindTexture(const int nTextureID)
{
	if (nTextureID >= NONE_IDX)
	{ // テクスチャインデックスが使用可能な場合

		for (int i = 0; i < COVER_MAX; i++)
		{ // 蓋の総数分繰り返す

			// テクスチャインデックスを割当
			m_apCover[i]->BindTexture(nTextureID);
		}

		// テクスチャインデックスを割当
		m_pCylinder->BindTexture(nTextureID);
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	テクスチャ割当処理 (パス)
//============================================================
void CObjectMeshTube::BindTexture(const char *pTexturePass)
{
	if (pTexturePass != nullptr)
	{ // 割り当てるテクスチャパスがある場合

		CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ
		for (int i = 0; i < COVER_MAX; i++)
		{ // 蓋の総数分繰り返す

			// テクスチャインデックスを割当
			m_apCover[i]->BindTexture(pTexture->Regist(pTexturePass));
		}

		// テクスチャインデックスを割当
		m_pCylinder->BindTexture(pTexture->Regist(pTexturePass));
	}
	else
	{ // 割り当てるテクスチャパスがない場合

		for (int i = 0; i < COVER_MAX; i++)
		{ // 蓋の総数分繰り返す

			// テクスチャなしインデックスを割当
			m_apCover[i]->BindTexture(NONE_IDX);
		}

		// テクスチャなしインデックスを割当
		m_pCylinder->BindTexture(NONE_IDX);
	}
}

//============================================================
//	色の設定処理
//============================================================
void CObjectMeshTube::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を設定
	m_pCylinder->SetColor(rCol);

	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		// 引数の色を設定
		m_apCover[i]->SetColor(rCol);
	}
}

//============================================================
//	半径の設定処理
//============================================================
void CObjectMeshTube::SetRadius(const float fRadius)
{
	// 引数の半径を設定
	m_pCylinder->SetRadius(fRadius);

	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		// 引数の半径を設定
		m_apCover[i]->SetRadius(fRadius);
	}
}

//============================================================
//	縦幅の設定処理
//============================================================
void CObjectMeshTube::SetHeight(const float fHeight)
{
	// 引数の縦幅を設定
	m_pCylinder->SetHeight(fHeight);

	// 相対位置を設定
	SetPositionRelative();
}

//============================================================
//	分割数の設定処理
//============================================================
HRESULT CObjectMeshTube::SetPattern(const POSGRID2& rPart)
{
	// 引数の分割数を設定
	if (FAILED(m_pCylinder->SetPattern(rPart)))
	{ // 設定に失敗した場合

		// 失敗を返す
		assert(false);
		return S_OK;
	}

	for (int i = 0; i < COVER_MAX; i++)
	{ // 蓋の総数分繰り返す

		// 引数の分割数を設定
		if (FAILED(m_apCover[i]->SetPattern(rPart)))
		{ // 設定に失敗した場合

			// 失敗を返す
			assert(false);
			return S_OK;
		}
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	テクスチャ分割数の設定処理
//============================================================
void CObjectMeshTube::SetTexPattern(const POSGRID2& rTexPart)
{
	if (rTexPart.x >= MIN_TEXPART.x
	&&  rTexPart.y >= MIN_TEXPART.y)
	{ // テクスチャ分割数が最低値以上の場合

		// 引数のテクスチャ分割数を設定
		m_pCylinder->SetTexPattern(rTexPart);
	}
	else { assert(false); }	// 最低値未満
}

//============================================================
//	相対位置の設定処理
//============================================================
void CObjectMeshTube::SetPositionRelative(void)
{
	D3DXMATRIX  mtxOffset, mtxBottom, mtxTop;	// マトリックス計算用
	D3DXMATRIX  mtxCylinder = CalcCylinderMtxWorld();	// シリンダーマトリックス
	D3DXVECTOR3 posCylinder = m_pCylinder->GetVec3Position();	// シリンダー位置
	D3DXVECTOR3 rotCylinder = m_pCylinder->GetVec3Rotation();	// シリンダー向き
	float fHeightCylinder   = m_pCylinder->GetHeight();			// シリンダー縦幅

	// 向きオフセットマトリックスを作成・掛け合わせる
	D3DXMatrixRotationYawPitchRoll(&mtxOffset, 0.0f, D3DX_PI, 0.0f);
	D3DXMatrixMultiply(&mtxBottom, &mtxOffset, &mtxCylinder);

	// 下蓋の位置・向きを設定
	m_apCover[COVER_BOTTOM]->SetVec3Position(posCylinder);
	m_apCover[COVER_BOTTOM]->SetVec3Rotation(useful::GetMatrixRotation(mtxBottom));

	// 位置オフセットマトリックスを作成・掛け合わせる
	D3DXMatrixTranslation(&mtxOffset, 0.0f, fHeightCylinder, 0.0f);
	D3DXMatrixMultiply(&mtxTop, &mtxOffset, &mtxCylinder);

	// 上蓋の位置・向きを設定
	m_apCover[COVER_TOP]->SetVec3Position(useful::GetMatrixPosition(mtxTop));
	m_apCover[COVER_TOP]->SetVec3Rotation(rotCylinder);
}

//============================================================
//	シリンダーマトリックスの計算結果の取得処理
//============================================================
D3DXMATRIX CObjectMeshTube::CalcCylinderMtxWorld(void) const
{
	// 変数を宣言
	D3DXMATRIX  mtxRot, mtxTrans, mtxWorld;	// 計算用マトリックス
	D3DXVECTOR3 posCylinder = m_pCylinder->GetVec3Position();	// シリンダー位置
	D3DXVECTOR3 rotCylinder = m_pCylinder->GetVec3Rotation();	// シリンダー向き

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rotCylinder.y, rotCylinder.x, rotCylinder.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, posCylinder.x, posCylinder.y, posCylinder.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	// ワールドマトリックスを返す
	return mtxWorld;
}
