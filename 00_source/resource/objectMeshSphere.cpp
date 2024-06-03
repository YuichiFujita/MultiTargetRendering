//============================================================
//
//	オブジェクトメッシュスフィア処理 [objectMeshSphere.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshSphere.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "objectMeshDome.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const POSGRID2 MIN_PART		= POSGRID2(1, 2);	// 分割数の最小値
	const POSGRID2 MIN_TEXPART	= GRID2_ONE;		// テクスチャ分割数の最小値
}

//************************************************************
//	子クラス [CObjectMeshSphere] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectMeshSphere::CObjectMeshSphere(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_part		(GRID2_ZERO),	// 分割数
	m_texPart	(GRID2_ZERO)	// テクスチャ分割数
{
	// メンバ変数をクリア
	memset(&m_apDome[0], 0, sizeof(m_apDome));	// 半球の情報
}

//============================================================
//	デストラクタ
//============================================================
CObjectMeshSphere::~CObjectMeshSphere()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectMeshSphere::Init(void)
{
	// メンバ変数を初期化
	memset(&m_apDome[0], 0, sizeof(m_apDome));	// 半球の情報
	m_part		= MIN_PART;		// 分割数
	m_texPart	= MIN_TEXPART;	// テクスチャ分割数

	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// メッシュドームの生成
		m_apDome[i] = CObjectMeshDome::Create
		( // 引数
			VEC3_ZERO,		// 位置
			VEC3_ZERO,		// 向き
			XCOL_WHITE,		// 色
			MIN_PART,		// 分割数
			MIN_TEXPART,	// テクスチャ分割数
			0.0f			// 半径
		);

		// 自動更新・自動描画をOFFにする
		m_apDome[i]->SetEnableUpdate(false);
		m_apDome[i]->SetEnableDraw(false);
	}

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
void CObjectMeshSphere::Uninit(void)
{
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// メッシュドームの終了
		SAFE_UNINIT(m_apDome[i]);
	}

	// オブジェクトメッシュスフィアを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectMeshSphere::Update(const float fDeltaTime)
{
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// メッシュドームの更新
		m_apDome[i]->Update(fDeltaTime);
	}
}

//============================================================
//	描画処理
//============================================================
void CObjectMeshSphere::Draw(CShader *pShader)
{
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// メッシュドームの描画
		m_apDome[i]->Draw(pShader);
	}
}

//============================================================
//	優先順位の設定処理
//============================================================
void CObjectMeshSphere::SetPriority(const int nPriority)
{
	// 引数の優先順位を設定
	CObject::SetPriority(nPriority);	// 自身
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		m_apDome[i]->SetPriority(nPriority);	// 半球
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CObjectMeshSphere::SetVec3Position(const D3DXVECTOR3& rPos)
{
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// 引数の位置を設定
		m_apDome[i]->SetVec3Position(rPos);
	}
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectMeshSphere::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// 引数の向きを設定
		D3DXVECTOR3 rot = rRot;
		rot.x += ((float)i * D3DX_PI);
		m_apDome[i]->SetVec3Rotation(rot);
	}
}

//============================================================
//	生成処理
//============================================================
CObjectMeshSphere *CObjectMeshSphere::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart,		// 分割数
	const POSGRID2& rTexPart,	// テクスチャ分割数
	const float fRadius			// 半径
)
{
	// オブジェクトメッシュスフィアの生成
	CObjectMeshSphere *pMeshSphere = new CObjectMeshSphere;
	if (pMeshSphere == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクトメッシュスフィアの初期化
		if (FAILED(pMeshSphere->Init()))
		{ // 初期化に失敗した場合

			// オブジェクトメッシュスフィアの破棄
			SAFE_DELETE(pMeshSphere);
			return nullptr;
		}

		// 位置を設定
		pMeshSphere->SetVec3Position(rPos);

		// 向きを設定
		pMeshSphere->SetVec3Rotation(rRot);

		// 色を設定
		pMeshSphere->SetColor(rCol);

		// 半径を設定
		pMeshSphere->SetRadius(fRadius);

		// 分割数を設定
		if (FAILED(pMeshSphere->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// オブジェクトメッシュスフィアの破棄
			SAFE_DELETE(pMeshSphere);
			return nullptr;
		}

		// テクスチャ分割数を設定
		pMeshSphere->SetTexPattern(rTexPart);

		// 確保したアドレスを返す
		return pMeshSphere;
	}
}

//============================================================
//	レンダーステート情報の設定処理
//============================================================
void CObjectMeshSphere::SetRenderState(CRenderState renderState)
{
	CRenderState *pTempRenderState = nullptr;	// レンダーステート情報
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// 引数のレンダーステートを設定
		pTempRenderState = m_apDome[i]->GetRenderState();
		*pTempRenderState = renderState;
	}
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObjectMeshSphere::BindTexture(const int nTextureID)
{
	if (nTextureID >= NONE_IDX)
	{ // テクスチャインデックスが使用可能な場合

		for (int i = 0; i < DOME_MAX; i++)
		{ // 半球の総数分繰り返す

			// テクスチャインデックスを割当
			m_apDome[i]->BindTexture(nTextureID);
		}
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	テクスチャ割当処理 (パス)
//============================================================
void CObjectMeshSphere::BindTexture(const char *pTexturePass)
{
	if (pTexturePass != nullptr)
	{ // 割り当てるテクスチャパスがある場合

		CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ
		for (int i = 0; i < DOME_MAX; i++)
		{ // 半球の総数分繰り返す

			// テクスチャインデックスを割当
			m_apDome[i]->BindTexture(pTexture->Regist(pTexturePass));
		}
	}
	else
	{ // 割り当てるテクスチャパスがない場合

		for (int i = 0; i < DOME_MAX; i++)
		{ // 半球の総数分繰り返す

			// テクスチャなしインデックスを割当
			m_apDome[i]->BindTexture(NONE_IDX);
		}
	}
}

//============================================================
//	色の設定処理
//============================================================
void CObjectMeshSphere::SetColor(const D3DXCOLOR& rCol)
{
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// 引数の色を設定
		m_apDome[i]->SetColor(rCol);
	}
}

//============================================================
//	半径の設定処理
//============================================================
void CObjectMeshSphere::SetRadius(const float fRadius)
{
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// 引数の半径を設定
		m_apDome[i]->SetRadius(fRadius);
	}
}

//============================================================
//	分割数の設定処理
//============================================================
HRESULT CObjectMeshSphere::SetPattern(const POSGRID2& rPart)
{
	for (int i = 0; i < DOME_MAX; i++)
	{ // 半球の総数分繰り返す

		// 引数の分割数を設定
		if (FAILED(m_apDome[i]->SetPattern(rPart)))
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
void CObjectMeshSphere::SetTexPattern(const POSGRID2& rTexPart)
{
	if (rTexPart.x >= MIN_TEXPART.x
	&&  rTexPart.y >= MIN_TEXPART.y)
	{ // テクスチャ分割数が最低値以上の場合

		for (int i = 0; i < DOME_MAX; i++)
		{ // 半球の総数分繰り返す

			// 引数のテクスチャ分割数を設定
			m_apDome[i]->SetTexPattern(rTexPart);
		}
	}
	else { assert(false); }	// 最低値未満
}
