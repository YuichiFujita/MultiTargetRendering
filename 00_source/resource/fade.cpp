//============================================================
//
//	フェード処理 [fade.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "fade.h"
#include "manager.h"
#include "renderer.h"
#include "loading.h"
#include "object2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int	PRIORITY	= 7;		// フェードの優先順位
	const float	FADE_LEVEL	= 0.05f;	// フェードのα値の加減量

#ifdef _DEBUG

	const CScene::EMode INIT_SCENE = CScene::MODE_TITLE;	// 初期シーン

#else	// NDEBUG

	const CScene::EMode INIT_SCENE = CScene::MODE_TITLE;	// 初期シーン

#endif	// _DEBUG
}

//************************************************************
//	親クラス [CFade] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CFade::CFade() :
	m_pObject2D		(nullptr),				// フェード表示の情報
	m_fade			(FADE_NONE),			// フェード状態
	m_modeNext		(CScene::MODE_TITLE),	// 次のシーンモード
	m_nCounterWait	(0)						// 余韻管理カウンター
{

}

//============================================================
//	デストラクタ
//============================================================
CFade::~CFade()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CFade::Init(void)
{
	// メンバ変数を初期化
	m_pObject2D = nullptr;		// フェード表示の情報
	m_fade		= FADE_IN;		// フェード状態
	m_modeNext	= INIT_SCENE;	// 次のシーンモード
	m_nCounterWait = 0;			// 余韻管理カウンター

	// オブジェクト2Dの生成
	m_pObject2D = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		SCREEN_SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		XCOL_BLACK		// 色
	);
	if (m_pObject2D == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位の設定
	m_pObject2D->SetPriority(PRIORITY);

	// ラベル指定なしに設定
	m_pObject2D->SetLabel(CObject::LABEL_NONE);

	// シーンの初期化
	if (FAILED(GET_MANAGER->InitScene(m_modeNext)))
	{ // 設定に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CFade::Uninit(void)
{
	// オブジェクト2Dの終了
	SAFE_UNINIT(m_pObject2D);
}

//============================================================
//	更新処理
//============================================================
void CFade::Update(const float fDeltaTime)
{
	if (m_fade != FADE_NONE)
	{ // 何もしない状態ではない場合

		D3DXCOLOR colFade = m_pObject2D->GetColor();	// フェード色
		switch (m_fade)
		{ // フェード状態ごとの処理
		case FADE_WAIT:	// フェード余韻状態

			if (m_nCounterWait > 0)
			{ // カウンターが 0より大きい場合

				// カウンターを減算
				m_nCounterWait--;
			}
			else
			{ // カウンターが 0以下の場合

				// フェード状態を設定
				m_fade = FADE_OUT;	// フェードアウト状態
			}

			break;

		case FADE_IN:	// フェードイン状態

			if (GET_MANAGER->GetLoading()->GetState() == CLoading::LOAD_NONE)
			{ // ロードしていない場合

				// α値を減算
				colFade.a -= FADE_LEVEL;
				if (colFade.a <= 0.0f)
				{ // α値が 0.0fを下回った場合

					// α値を補正
					colFade.a = 0.0f;

					// フェード状態を設定
					m_fade = FADE_NONE;	// 何もしない状態
				}
			}

			break;

		case FADE_OUT:	// フェードアウト状態

			// α値を加算
			colFade.a += FADE_LEVEL;
			if (colFade.a >= 1.0f)
			{ // α値が 1.0f を上回った場合

				// α値を補正
				colFade.a = 1.0f;

				// フェード状態を設定
				m_fade = FADE_IN;	// フェードイン状態

				// モードの設定
				if (FAILED(GET_MANAGER->SetMode(m_modeNext)))
				{ // モードの設定に失敗した場合

					// ウインドウを破棄する
					CManager::ReleaseWindow();
				}
			}

			break;

		default:	// 例外処理
			assert(false);
			break;
		}

		// 色の更新
		m_pObject2D->SetColor(colFade);

		// オブジェクト2Dの更新
		m_pObject2D->Update(fDeltaTime);
	}
}

//============================================================
//	描画処理
//============================================================
void CFade::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// サンプラーステートを設定
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);	// U方向のラッピングを無効化
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);	// V方向のラッピングを無効化

	// オブジェクト2Dの描画
	m_pObject2D->Draw();

	// サンプラーステートを設定
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// U方向のラッピングを有効化
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// V方向のラッピングを有効化
}

//============================================================
//	次シーンへのフェード設定処理
//============================================================
void CFade::Set(const CScene::EMode mode, const int nWait)
{
	// 例外処理
	assert(nWait >= 0);	// 余韻フレームオーバー

	if (m_fade == FADE_NONE)
	{ // フェードが行われていない場合

		// 引数のモードを設定
		m_modeNext = mode;

		// 余韻管理カウンターを設定
		m_nCounterWait = nWait;

		// フェード状態を設定
		if (m_nCounterWait <= 0)
		{ // カウンターが 0以下の場合

			m_fade = FADE_OUT;	// フェードアウト状態
		}
		else
		{ // カウンターが 0より大きい場合

			m_fade = FADE_WAIT;	// フェード余韻状態
		}
	}
}

//============================================================
//	フェード状態取得処理
//============================================================
CFade::EFade CFade::GetState(void) const
{
	// 現在のフェード状態を返す
	return m_fade;
}

//============================================================
//	生成処理
//============================================================
CFade *CFade::Create(void)
{
	// フェードの生成
	CFade *pFade = new CFade;
	if (pFade == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// フェードの初期化
		if (FAILED(pFade->Init()))
		{ // 初期化に失敗した場合

			// フェードの破棄
			SAFE_DELETE(pFade);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pFade;
	}
}

//============================================================
//	破棄処理
//============================================================
void CFade::Release(CFade *&prFade)
{
	// フェードの終了
	assert(prFade != nullptr);
	prFade->Uninit();

	// メモリ開放
	SAFE_DELETE(prFade);
}
