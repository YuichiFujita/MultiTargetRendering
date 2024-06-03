//============================================================
//
//	数字UI処理 [valueUI.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "valueUI.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "object2D.h"
#include "multiValue.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 6;	// 数字UI表示の優先順位
}

//************************************************************
//	子クラス [CValueUI] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CValueUI::CValueUI() : CObject(CObject::LABEL_NONE, CObject::DIM_2D, PRIORITY),
	m_pTitle	(nullptr),		// タイトル情報
	m_pValue	(nullptr),		// 数字情報
	m_pos		(VEC3_ZERO),	// 位置
	m_space		(VEC3_ZERO)		// 行間
{

}

//============================================================
//	デストラクタ
//============================================================
CValueUI::~CValueUI()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CValueUI::Init(void)
{
	// メンバ変数を初期化
	m_pTitle	= nullptr;		// タイトル情報
	m_pValue	= nullptr;		// 数字情報
	m_pos		= VEC3_ZERO;	// 位置
	m_space		= VEC3_ZERO;	// 行間

	// タイトル情報の生成
	m_pTitle = CObject2D::Create(VEC3_ZERO);
	if (m_pTitle == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pTitle->SetPriority(PRIORITY);

	// 数字情報の生成
	m_pValue = CMultiValue::Create
	( // 引数
		CValue::TYPE_NORMAL,	// 数字種類
		0,			// 数字
		1,			// 桁数
		VEC3_ZERO,	// 位置
		VEC3_ONE,	// 大きさ
		VEC3_ZERO	// 列間
	);
	if (m_pValue == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pValue->SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CValueUI::Uninit(void)
{
	// タイトル情報の終了
	SAFE_UNINIT(m_pTitle);

	// 自身のオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CValueUI::Update(const float fDeltaTime)
{
	// タイトル情報の更新
	m_pTitle->Update(fDeltaTime);

	// 数字情報の更新
	m_pValue->Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CValueUI::Draw(CShader * /*pShader*/)
{

}

//============================================================
//	優先順位の設定処理
//============================================================
void CValueUI::SetPriority(const int nPriority)
{
	// 引数の優先順位を設定
	CObject::SetPriority(nPriority);	// 自身
	m_pTitle->SetPriority(nPriority);	// タイトル情報
	m_pValue->SetPriority(nPriority);	// 数字情報
}

//============================================================
//	描画状況の設定処理
//============================================================
void CValueUI::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);	// 自身
	m_pTitle->SetEnableDraw(bDraw);	// タイトル情報
	m_pValue->SetEnableDraw(bDraw);	// 数字情報
}

//============================================================
//	位置の設定処理
//============================================================
void CValueUI::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	生成処理 (テクスチャ指定なし)
//============================================================
CValueUI *CValueUI::Create
(
	const CValue::EType type,		// 数字種類
	const int nDigit,				// 桁数
	const D3DXVECTOR3& rPos,		// 位置
	const D3DXVECTOR3& rSpace,		// 行間
	const D3DXVECTOR3& rSpaceValue,	// 数字行間
	const D3DXVECTOR3& rSizeTitle,	// タイトル大きさ
	const D3DXVECTOR3& rSizeValue,	// 数字大きさ
	const D3DXVECTOR3& rRotTitle,	// タイトル向き
	const D3DXVECTOR3& rRotValue,	// 数字向き
	const D3DXCOLOR& rColTitle,		// タイトル色
	const D3DXCOLOR& rColValue		// 数字色
)
{
	// 数字UIの生成
	CValueUI *pValueUI = new CValueUI;
	if (pValueUI == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		CMultiValue *pMultiModel = pValueUI->GetMultiValue();	// 数字オブジェクト

		// 数字UIの初期化
		if (FAILED(pValueUI->Init()))
		{ // 初期化に失敗した場合

			// 数字UIの破棄
			SAFE_DELETE(pValueUI);
			return nullptr;
		}

		// 数字の桁数を設定
		pMultiModel->SetDigit(nDigit);

		// 数字テクスチャを設定
		pMultiModel->SetType(type);

		// タイトル向きを設定
		pValueUI->SetRotationTitle(rRotTitle);

		// 数字向きを設定
		pMultiModel->SetVec3Rotation(rRotValue);

		// タイトル大きさを設定
		pValueUI->SetSizingTitle(rSizeTitle);

		// 数字大きさを設定
		pMultiModel->SetVec3Sizing(rSizeValue);

		// タイトル色を設定
		pValueUI->SetColorTitle(rColTitle);

		// 数字色を設定
		pMultiModel->SetColor(rColValue);

		// 数字の行間を設定
		pMultiModel->SetSpace(rSpaceValue);

		// 行間を設定
		pValueUI->SetSpace(rSpace);

		// 位置を設定
		pValueUI->SetVec3Position(rPos);

		// 確保したアドレスを返す
		return pValueUI;
	}
}

//============================================================
//	生成処理 (テクスチャインデックス指定)
//============================================================
CValueUI *CValueUI::Create
(
	const int nTextureID,			// タイトルテクスチャインデックス
	const CValue::EType type,		// 数字種類
	const int nDigit,				// 桁数
	const D3DXVECTOR3& rPos,		// 位置
	const D3DXVECTOR3& rSpace,		// 行間
	const D3DXVECTOR3& rSpaceValue,	// 数字行間
	const D3DXVECTOR3& rSizeTitle,	// タイトル大きさ
	const D3DXVECTOR3& rSizeValue,	// 数字大きさ
	const D3DXVECTOR3& rRotTitle,	// タイトル向き
	const D3DXVECTOR3& rRotValue,	// 数字向き
	const D3DXCOLOR& rColTitle,		// タイトル色
	const D3DXCOLOR& rColValue		// 数字色
)
{
	// 数字UIの生成
	CValueUI *pValueUI = CValueUI::Create
	( // 引数
		type,			// 数字種類
		nDigit,			// 桁数
		rPos,			// 位置
		rSpace,			// 行間
		rSpaceValue,	// 数字行間
		rSizeTitle,		// タイトル大きさ
		rSizeValue,		// 数字大きさ
		rRotTitle,		// タイトル向き
		rRotValue,		// 数字向き
		rColTitle,		// タイトル色
		rColValue		// 数字色
	);
	if (pValueUI == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// テクスチャを割当
		pValueUI->BindTextureTitle(nTextureID);

		// 確保したアドレスを返す
		return pValueUI;
	}
}

//============================================================
//	生成処理 (テクスチャパス指定)
//============================================================
CValueUI *CValueUI::Create
(
	const char *pTexturePass,		// タイトルテクスチャパス
	const CValue::EType type,		// 数字種類
	const int nDigit,				// 桁数
	const D3DXVECTOR3& rPos,		// 位置
	const D3DXVECTOR3& rSpace,		// 行間
	const D3DXVECTOR3& rSpaceValue,	// 数字行間
	const D3DXVECTOR3& rSizeTitle,	// タイトル大きさ
	const D3DXVECTOR3& rSizeValue,	// 数字大きさ
	const D3DXVECTOR3& rRotTitle,	// タイトル向き
	const D3DXVECTOR3& rRotValue,	// 数字向き
	const D3DXCOLOR& rColTitle,		// タイトル色
	const D3DXCOLOR& rColValue		// 数字色
)
{
	// 数字UIの生成
	CValueUI *pValueUI = CValueUI::Create
	( // 引数
		type,			// 数字種類
		nDigit,			// 桁数
		rPos,			// 位置
		rSpace,			// 行間
		rSpaceValue,	// 数字行間
		rSizeTitle,		// タイトル大きさ
		rSizeValue,		// 数字大きさ
		rRotTitle,		// タイトル向き
		rRotValue,		// 数字向き
		rColTitle,		// タイトル色
		rColValue		// 数字色
	);
	if (pValueUI == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// テクスチャを割当
		pValueUI->BindTextureTitle(pTexturePass);

		// 確保したアドレスを返す
		return pValueUI;
	}
}

//============================================================
//	行間の設定処理
//============================================================
void CValueUI::SetSpace(const D3DXVECTOR3& rSpace)
{
	// 引数の行間を設定
	m_space = rSpace;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	タイトルのテクスチャ割当処理 (インデックス)
//============================================================
void CValueUI::BindTextureTitle(const int nTextureID)
{
	if (nTextureID >= NONE_IDX)
	{ // テクスチャインデックスが使用可能な場合

		// テクスチャインデックスを代入
		m_pTitle->BindTexture(nTextureID);
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	タイトルのテクスチャ割当処理 (パス)
//============================================================
void CValueUI::BindTextureTitle(const char *pTexturePass)
{
	// ポインタを宣言
	CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ

	if (pTexturePass != nullptr)
	{ // 割り当てるテクスチャパスがある場合

		// テクスチャインデックスを設定
		m_pTitle->BindTexture(pTexture->Regist(pTexturePass));
	}
	else
	{ // 割り当てるテクスチャパスがない場合

		// テクスチャなしインデックスを設定
		m_pTitle->BindTexture(NONE_IDX);
	}
}

//============================================================
//	タイトル向きの設定処理
//============================================================
void CValueUI::SetRotationTitle(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_pTitle->SetVec3Rotation(rRot);
}

//============================================================
//	タイトル大きさの設定処理
//============================================================
void CValueUI::SetSizingTitle(const D3DXVECTOR3& rSize)
{
	// 引数の大きさを設定
	m_pTitle->SetVec3Sizing(rSize);
}

//============================================================
//	タイトル色の設定処理
//============================================================
void CValueUI::SetColorTitle(const D3DXCOLOR& rCol)
{
	// 引数の色を設定
	m_pTitle->SetColor(rCol);
}

//============================================================
//	相対位置の設定処理
//============================================================
void CValueUI::SetPositionRelative(void)
{
	// タイトルの位置を設定
	m_pTitle->SetVec3Position(m_pos);

	// 数字の位置を設定
	m_pValue->SetVec3Position(m_pTitle->GetVec3Position() + m_space);
}
