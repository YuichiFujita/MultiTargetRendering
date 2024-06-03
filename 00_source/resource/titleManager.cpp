//============================================================
//
//	タイトルマネージャー処理 [titleManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "titleManager.h"
#include "manager.h"
#include "sound.h"
#include "camera.h"
#include "fade.h"
#include "texture.h"
#include "model.h"
#include "object2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEX_LOGO_FILE[] =	// ロゴテクスチャファイル
	{
		"data\\TEXTURE\\title000.png",	// NEVERテクスチャ
		"data\\TEXTURE\\title001.png",	// GIVEテクスチャ
		"data\\TEXTURE\\title002.png",	// UP!テクスチャ
	};

	const char *TEX_SELECT_FILE[] =	// 選択項目テクスチャファイル
	{
		"data\\TEXTURE\\title003.png",	// STARTテクスチャ
		"data\\TEXTURE\\title004.png",	// MANUALテクスチャ
	};

	const int PRIORITY = 5;	// タイトルの優先順位

	namespace fade
	{
		const D3DXCOLOR COL = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);	// タイトルフェードの色

		const float SUB_ALPHA = 0.008f;	// α値の減算量
	}

	namespace logo
	{
		const D3DXVECTOR3 POS_NEVER	= D3DXVECTOR3(340.0f,  140.0f, 0.0f);	// タイトルロゴの位置 (NEVER)
		const D3DXVECTOR3 POS_GIVE	= D3DXVECTOR3(710.0f,  310.0f, 0.0f);	// タイトルロゴの位置 (GIVE)
		const D3DXVECTOR3 POS_UP	= D3DXVECTOR3(1050.0f, 320.0f, 0.0f);	// タイトルロゴの位置 (UP!)

		const D3DXVECTOR3 POS[] =	// 位置配列
		{
			POS_NEVER,	// NEVERの位置
			POS_GIVE,	// GIVEの位置
			POS_UP,		// UP!の位置
		};

		const D3DXVECTOR3 SIZE	= D3DXVECTOR3(666.0f, 290.0f, 0.0f) * 0.8f;	// タイトルロゴの大きさ
		const float	INIT_SCALE	= 15.0f;	// タイトルロゴの初期拡大率
		const float	SUB_SCALE	= 0.65f;	// タイトルロゴ拡大率の減算量
	}

	namespace select
	{
		const D3DXVECTOR3 POS	= D3DXVECTOR3(350.0f, 560.0f, 0.0f);	// 選択の位置
		const D3DXVECTOR3 SPACE	= D3DXVECTOR3(560.0f, 0.0f, 0.0f);		// 選択の空間
		const D3DXVECTOR3 SIZE	= D3DXVECTOR3(420.0f, 140.0f, 0.0f);	// 選択の大きさ

		const D3DXCOLOR CHOICE_COL	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 選択中カラー
		const D3DXCOLOR DEFAULT_COL	= D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);	// 非選択中カラー
	}

	namespace selectBG
	{
		const D3DXVECTOR3 POS	= D3DXVECTOR3(640.0f, 560.0f, 0.0f);				// 選択背景の位置
		const D3DXVECTOR3 SIZE	= D3DXVECTOR3((float)SCREEN_WIDTH, 120.0f, 0.0f);	// 選択背景の大きさ

		const D3DXCOLOR COL = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);	// 選択背景の色
	}
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEX_LOGO_FILE)   == CTitleManager::LOGO_MAX,   "ERROR : LogoTexture Count Mismatch");
static_assert(NUM_ARRAY(TEX_SELECT_FILE) == CTitleManager::SELECT_MAX, "ERROR : SelectTexture Count Mismatch");

//************************************************************
//	親クラス [CTitleManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CTitleManager::CTitleManager() :
	m_pFade			(nullptr),		// フェードの情報
	m_pSelectBG		(nullptr),		// 選択背景の情報
	m_state			(STATE_NONE),	// 状態
	m_fScale		(0.0f),			// タイトル拡大率
	m_nSelect		(0),			// 現在の選択
	m_nOldSelect	(0)				// 前回の選択
{
	// メンバ変数をクリア
	memset(&m_apLogo[0],	0, sizeof(m_apLogo));	// タイトル表示の情報
	memset(&m_apSelect[0],	0, sizeof(m_apSelect));	// 選択表示の情報
}

//============================================================
//	デストラクタ
//============================================================
CTitleManager::~CTitleManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CTitleManager::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	memset(&m_apLogo[0],	0, sizeof(m_apLogo));	// タイトル表示の情報
	memset(&m_apSelect[0],	0, sizeof(m_apSelect));	// 選択表示の情報
	m_pFade			= nullptr;			// フェードの情報
	m_pSelectBG		= nullptr;			// 選択背景の情報
	m_state			= STATE_FADEOUT;	// 状態
	m_fScale		= logo::INIT_SCALE;	// タイトル拡大率
	m_nSelect		= 0;				// 現在の選択
	m_nOldSelect	= 0;				// 前回の選択

	//--------------------------------------------------------
	//	選択背景の生成・設定
	//--------------------------------------------------------
	// 選択背景の生成
	m_pSelectBG = CObject2D::Create
	( // 引数
		selectBG::POS,	// 位置
		selectBG::SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		selectBG::COL	// 色
	);
	if (m_pSelectBG == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pSelectBG->SetPriority(PRIORITY);

	// 描画をしない設定にする
	m_pSelectBG->SetEnableDraw(false);

	//--------------------------------------------------------
	//	選択表示の生成・設定
	//--------------------------------------------------------
	for (int nCntTitle = 0; nCntTitle < SELECT_MAX; nCntTitle++)
	{ // 選択項目の総数分繰り返す

		// 選択表示の生成
		m_apSelect[nCntTitle] = CObject2D::Create
		( // 引数
			select::POS + (select::SPACE * (float)nCntTitle),	// 位置
			select::SIZE,			// 大きさ
			VEC3_ZERO,				// 向き
			select::DEFAULT_COL		// 色
		);
		if (m_apSelect[nCntTitle] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録・割当
		m_apSelect[nCntTitle]->BindTexture(pTexture->Regist(TEX_SELECT_FILE[nCntTitle]));

		// 優先順位を設定
		m_apSelect[nCntTitle]->SetPriority(PRIORITY);

		// 描画をしない設定にする
		m_apSelect[nCntTitle]->SetEnableDraw(false);
	}

	//--------------------------------------------------------
	//	タイトルロゴの生成・設定
	//--------------------------------------------------------
	for (int nCntTitle = 0; nCntTitle < LOGO_MAX; nCntTitle++)
	{ // タイトルロゴの総数分繰り返す

		// タイトルロゴの生成
		m_apLogo[nCntTitle] = CObject2D::Create
		( // 引数
			logo::POS[nCntTitle],	// 位置
			logo::SIZE				// 大きさ
		);
		if (m_apLogo[nCntTitle] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録・割当
		m_apLogo[nCntTitle]->BindTexture(pTexture->Regist(TEX_LOGO_FILE[nCntTitle]));

		// 優先順位を設定
		m_apLogo[nCntTitle]->SetPriority(PRIORITY);

		// 描画をしない設定にする
		m_apLogo[nCntTitle]->SetEnableDraw(false);
	}

	//--------------------------------------------------------
	//	フェードの生成・設定
	//--------------------------------------------------------
	// フェードの生成
	m_pFade = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		SCREEN_SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		fade::COL		// 色
	);
	if (m_pFade == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pFade->SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CTitleManager::Uninit(void)
{
	for (int nCntTitle = 0; nCntTitle < LOGO_MAX; nCntTitle++)
	{ // タイトルロゴの総数分繰り返す

		// タイトルロゴの終了
		SAFE_UNINIT(m_apLogo[nCntTitle]);
	}

	for (int nCntTitle = 0; nCntTitle < SELECT_MAX; nCntTitle++)
	{ // 選択項目の総数分繰り返す

		// 選択表示の終了
		SAFE_UNINIT(m_apSelect[nCntTitle]);
	}

	// フェードの終了
	SAFE_UNINIT(m_pFade);

	// 選択背景の終了
	SAFE_UNINIT(m_pSelectBG);
}

//============================================================
//	更新処理
//============================================================
void CTitleManager::Update(const float fDeltaTime)
{
	// 遷移決定の更新
	UpdateStart();

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:	// 何もしない状態

		// 無し

		break;

	case STATE_FADEOUT:	// フェードアウト状態

		// フェードアウトの更新
		UpdateFade();

		break;

	case STATE_MOVE:	// タイトル縮小状態

		// タイトル移動の更新
		UpdateMove();

		break;

	case STATE_WAIT:	// 遷移待機状態

		// 選択操作
		ActSelect();

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	for (int nCntTitle = 0; nCntTitle < LOGO_MAX; nCntTitle++)
	{ // タイトルロゴの総数分繰り返す

		// タイトルロゴの更新
		m_apLogo[nCntTitle]->Update(fDeltaTime);
	}

	for (int nCntTitle = 0; nCntTitle < SELECT_MAX; nCntTitle++)
	{ // 選択項目の総数分繰り返す

		// 選択表示の更新
		m_apSelect[nCntTitle]->Update(fDeltaTime);
	}

	// フェードの更新
	m_pFade->Update(fDeltaTime);

	// 選択背景の更新
	m_pSelectBG->Update(fDeltaTime);
}

//============================================================
//	生成処理
//============================================================
CTitleManager *CTitleManager::Create(void)
{
	// タイトルマネージャーの生成
	CTitleManager *pTitleManager = new CTitleManager;
	if (pTitleManager == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// タイトルマネージャーの初期化
		if (FAILED(pTitleManager->Init()))
		{ // 初期化に失敗した場合

			// タイトルマネージャーの破棄
			SAFE_DELETE(pTitleManager);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pTitleManager;
	}
}

//============================================================
//	破棄処理
//============================================================
void CTitleManager::Release(CTitleManager *&prTitleManager)
{
	// タイトルマネージャーの終了
	assert(prTitleManager != nullptr);
	prTitleManager->Uninit();

	// メモリ開放
	SAFE_DELETE(prTitleManager);
}

//============================================================
//	フェードアウトの更新処理
//============================================================
void CTitleManager::UpdateFade(void)
{
	// 変数を宣言
	D3DXCOLOR colFade = m_pFade->GetColor();	// フェードの色

	if (colFade.a > 0.0f)
	{ // 透明になっていない場合

		// 透明度を減算
		colFade.a -= fade::SUB_ALPHA;
	}
	else
	{ // 透明になった場合

		// 透明度を補正
		colFade.a = 0.0f;

		for (int nCntTitle = 0; nCntTitle < LOGO_MAX; nCntTitle++)
		{ // タイトルロゴの総数分繰り返す

			// 描画をする設定にする
			m_apLogo[nCntTitle]->SetEnableDraw(true);
		}

		for (int nCntTitle = 0; nCntTitle < SELECT_MAX; nCntTitle++)
		{ // 選択項目の総数分繰り返す

			// 選択表示を描画する設定にする
			m_apSelect[nCntTitle]->SetEnableDraw(true);
		}

		// 選択背景を描画する設定にする
		m_pSelectBG->SetEnableDraw(true);

		// 状態を変更
		m_state = STATE_MOVE;	// タイトル移動状態
	}

	// 透明度を反映
	m_pFade->SetColor(colFade);
}

//============================================================
//	タイトル移動の更新処理
//============================================================
void CTitleManager::UpdateMove(void)
{
	if (m_fScale > 1.0f)
	{ // 拡大率が最小値より大きい場合

		// 拡大率を減算
		m_fScale -= logo::SUB_SCALE;

		for (int nCntTitle = 0; nCntTitle < LOGO_MAX; nCntTitle++)
		{ // タイトルロゴの総数分繰り返す

			// タイトルロゴの大きさを設定
			m_apLogo[nCntTitle]->SetVec3Sizing(logo::SIZE * m_fScale);
		}
	}
	else
	{ // 拡大率が最小値以下の場合

		// 拡大率を補正
		m_fScale = 1.0f;

		for (int nCntTitle = 0; nCntTitle < LOGO_MAX; nCntTitle++)
		{ // タイトルロゴの総数分繰り返す

			// タイトルロゴの大きさを設定
			m_apLogo[nCntTitle]->SetVec3Sizing(logo::SIZE);
		}

		// 状態を変更
		m_state = STATE_WAIT;	// 遷移待機状態

		// カメラの更新を再開
		GET_MANAGER->GetCamera()->SetEnableUpdate(true);

		// サウンドの再生
		GET_MANAGER->GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
	}
}

//============================================================
//	遷移決定の更新処理
//============================================================
void CTitleManager::UpdateStart(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= GET_INPUTKEY;	// キーボード
	CInputPad		*pPad		= GET_INPUTPAD;	// パッド

	if (pKeyboard->IsTrigger(DIK_RETURN)
	||  pKeyboard->IsTrigger(DIK_SPACE)
	||  pPad->IsTrigger(CInputPad::KEY_A)
	||  pPad->IsTrigger(CInputPad::KEY_B)
	||  pPad->IsTrigger(CInputPad::KEY_X)
	||  pPad->IsTrigger(CInputPad::KEY_Y)
	||  pPad->IsTrigger(CInputPad::KEY_START))
	{
		if (m_state != STATE_WAIT)
		{ // 遷移待機状態ではない場合

			// 演出スキップ
			SkipStaging();

			// サウンドの再生
			GET_MANAGER->GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
		}
		else
		{ // 遷移待機状態の場合

			if (GET_MANAGER->GetFade()->GetState() == CFade::FADE_NONE)
			{ // フェード中ではない場合

				switch (m_nSelect)
				{ // 選択ごとの処理
				case SELECT_GAME:

					// シーンの設定
					GET_MANAGER->SetFadeScene(CScene::MODE_GAME);	// ゲーム画面

					break;

				case SELECT_TUTORIAL:

					// シーンの設定
					GET_MANAGER->SetFadeScene(CScene::MODE_TUTORIAL);	// チュートリアル画面

					break;

				default:
					assert(false);
					break;
				}

				// サウンドの再生
				GET_MANAGER->GetSound()->Play(CSound::LABEL_SE_DECISION_000);	// 決定音00
			}
		}
	}
}

//============================================================
//	選択操作処理
//============================================================
void CTitleManager::ActSelect(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= GET_INPUTKEY;	// キーボード
	CInputPad		*pPad		= GET_INPUTPAD;		// パッド

	if (GET_MANAGER->GetFade()->GetState() == CFade::FADE_NONE)
	{ // フェード中ではない場合

		if (pKeyboard->IsTrigger(DIK_A)
		||  pKeyboard->IsTrigger(DIK_LEFT)
		||  pPad->IsTrigger(CInputPad::KEY_LEFT))
		{ // 左移動の操作が行われた場合

			// 左に選択をずらす
			m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX;

			// サウンドの再生
			GET_MANAGER->GetSound()->Play(CSound::LABEL_SE_SELECT_000);	// 選択操作音00
		}
		if (pKeyboard->IsTrigger(DIK_D)
		||  pKeyboard->IsTrigger(DIK_RIGHT)
		||  pPad->IsTrigger(CInputPad::KEY_RIGHT))
		{ // 右移動の操作が行われた場合

			// 右に選択をずらす
			m_nSelect = (m_nSelect + 1) % SELECT_MAX;

			// サウンドの再生
			GET_MANAGER->GetSound()->Play(CSound::LABEL_SE_SELECT_000);	// 選択操作音00
		}

		// 前回の選択要素の色を黒に設定
		m_apSelect[m_nOldSelect]->SetColor(select::DEFAULT_COL);

		// 現在の選択要素の色を白に設定
		m_apSelect[m_nSelect]->SetColor(select::CHOICE_COL);

		// 現在の選択要素を代入
		m_nOldSelect = m_nSelect;
	}
}

//============================================================
//	演出スキップ処理
//============================================================
void CTitleManager::SkipStaging(void)
{
	// タイトルロゴを表示状態に設定・大きさを正規化
	for (int nCntTitle = 0; nCntTitle < LOGO_MAX; nCntTitle++)
	{ // タイトルロゴの総数分繰り返す

		// タイトルロゴの大きさを設定
		m_apLogo[nCntTitle]->SetVec3Sizing(logo::SIZE);

		// 描画をする設定にする
		m_apLogo[nCntTitle]->SetEnableDraw(true);
	}

	// 選択表示を描画する設定にする
	for (int nCntTitle = 0; nCntTitle < SELECT_MAX; nCntTitle++)
	{ // 選択項目の総数分繰り返す

		m_apSelect[nCntTitle]->SetEnableDraw(true);
	}

	// フェードを透明にする
	m_pFade->SetColor(XCOL_ABLACK);

	// 選択背景を描画する設定にする
	m_pSelectBG->SetEnableDraw(true);

	// カメラの更新を再開
	GET_MANAGER->GetCamera()->SetEnableUpdate(true);

	// 状態を変更
	m_state = STATE_WAIT;	// 遷移待機状態
}
