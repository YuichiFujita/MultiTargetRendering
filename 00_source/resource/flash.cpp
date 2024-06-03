//============================================================
//
//	フラッシュ処理 [flash.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "flash.h"
#include "object2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 7;	// フラッシュの優先順位
}

//************************************************************
//	親クラス [CFlash] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CFlash::CFlash() :
	m_pObject2D	(nullptr),		// フラッシュ表示の情報
	m_flash		(FLASH_NONE),	// フラッシュ状態
	m_fSubAlpha	(0.0f)			// 透明度の減算量
{

}

//============================================================
//	デストラクタ
//============================================================
CFlash::~CFlash()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CFlash::Init(void)
{
	// メンバ変数を初期化
	m_pObject2D = nullptr;		// フラッシュ表示の情報
	m_flash		= FLASH_IN;		// フラッシュ状態
	m_fSubAlpha	= 0.0f;			// 透明度の減算量

	// オブジェクト2Dの生成
	m_pObject2D = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		SCREEN_SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		XCOL_AWHITE		// 色
	);
	if (m_pObject2D == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位の設定
	m_pObject2D->SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CFlash::Uninit(void)
{
	// オブジェクト2Dの終了
	SAFE_UNINIT(m_pObject2D);
}

//============================================================
//	更新処理
//============================================================
void CFlash::Update(const float fDeltaTime)
{
	if (m_flash != FLASH_NONE)
	{ // 何もしない状態ではない場合

		// 変数を宣言
		D3DXCOLOR colFlash = m_pObject2D->GetColor();	// フラッシュ色

		switch (m_flash)
		{ // フラッシュ状態ごとの処理
		case FLASH_IN:	// フラッシュイン状態

			// α値を減算
			colFlash.a -= m_fSubAlpha;

			if (colFlash.a <= 0.0f)
			{ // α値が 0.0fを下回った場合

				// α値を補正
				colFlash.a = 0.0f;

				// フラッシュ状態を設定
				m_flash = FLASH_NONE;	// 何もしない状態
			}

			break;

		default:	// 例外処理
			assert(false);
			break;
		}

		// 色の反映
		m_pObject2D->SetColor(colFlash);

		// オブジェクト2Dの更新
		m_pObject2D->Update(fDeltaTime);
	}
}

//============================================================
//	フラッシュの設定処理
//============================================================
void CFlash::Set(const float fStartAlpha, const float fSubAlpha)
{
	// 初期の透明度を設定
	D3DXCOLOR colFlash = m_pObject2D->GetColor();	// フラッシュ色
	colFlash.a = fStartAlpha;			// 透明度を設定
	m_pObject2D->SetColor(colFlash);	// 透明度を反映

	// 透明度の減算量を設定
	m_fSubAlpha = fSubAlpha;

	// フラッシュイン状態にする
	m_flash = FLASH_IN;
}

//============================================================
//	生成処理
//============================================================
CFlash *CFlash::Create(void)
{
	// フラッシュの生成
	CFlash *pFlash = new CFlash;
	if (pFlash == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// フラッシュの初期化
		if (FAILED(pFlash->Init()))
		{ // 初期化に失敗した場合

			// フラッシュの破棄
			SAFE_DELETE(pFlash);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pFlash;
	}
}

//============================================================
//	破棄処理
//============================================================
void CFlash::Release(CFlash *&prFlash)
{
	// フラッシュの終了
	assert(prFlash != nullptr);
	prFlash->Uninit();

	// メモリ開放
	SAFE_DELETE(prFlash);
}
