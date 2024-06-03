//============================================================
//
//	リザルトマネージャー処理 [resultManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "resultManager.h"
#include "manager.h"
#include "sound.h"
#include "camera.h"
#include "fade.h"
#include "texture.h"
#include "model.h"
#include "object2D.h"
#include "timeUI.h"
#include "retentionManager.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャファイル
	{
		"data\\TEXTURE\\result000.png",		// ボス討伐テクスチャ
		"data\\TEXTURE\\result001.png",		// 成功テクスチャ
		"data\\TEXTURE\\result002.png",		// 失敗テクスチャ
		"data\\TEXTURE\\result003.png",		// タイム表示テクスチャ
		"data\\TEXTURE\\continue000.png",	// コンテニュー表示テクスチャ
		"data\\TEXTURE\\select000.png",		// YESテクスチャ
		"data\\TEXTURE\\select001.png",		// NOテクスチャ
	};

	const int PRIORITY = 5;	// リザルトの優先順位

	const D3DXCOLOR SETCOL_FADE		= D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);	// α値の停止値
	const D3DXCOLOR INITCOL_FADE	= XCOL_AWHITE;							// α値の初期値
	const D3DXVECTOR3 SIZE_FADE		= SCREEN_SIZE - D3DXVECTOR3(50.0f, 50.0f, 0.0f);	// フェードの大きさ

	const float ADD_ALPHA = 0.008f;	// α値の加算量

	const D3DXVECTOR3 POS_RESULT_MISSION	= D3DXVECTOR3(380.0f, 150.0f, 0.0f);	// リザルト表示の遅刻回避の位置
	const D3DXVECTOR3 POS_RESULT_RESULT		= D3DXVECTOR3(960.0f, 150.0f, 0.0f);	// リザルト表示の成功失敗の位置
	const D3DXVECTOR3 SIZE_RESULT			= D3DXVECTOR3(632.7f, 203.5f, 0.0f);	// リザルト表示の大きさ

	const float SET_RESULT_SCALE	= 15.0f;	// リザルト表示の初期拡大率
	const float SUB_RESULT_SCALE	= 0.65f;	// リザルト表示拡大率の減算量

	const D3DXVECTOR3 POS_TIME_LOGO		= D3DXVECTOR3(290.0f, 330.0f, 0.0f);	// タイムロゴ位置
	const D3DXVECTOR3 SIZE_TIME_LOGO	= D3DXVECTOR3(487.5f, 154.7f, 0.0f);	// タイムロゴ大きさ
	const D3DXVECTOR3 POS_TIME			= D3DXVECTOR3(845.0f, 330.0f, 0.0f);	// タイム位置
	const D3DXVECTOR3 SIZE_TIME_VAL		= D3DXVECTOR3(98.0f, 117.0f, 0.0f);		// タイム数字大きさ
	const D3DXVECTOR3 SIZE_TIME_PART	= D3DXVECTOR3(50.0f, 117.0f, 0.0f);		// タイム区切り大きさ
	const D3DXVECTOR3 SPACE_TIME_VAL	= D3DXVECTOR3(SIZE_TIME_VAL.x  * 0.85f, 0.0f, 0.0f);	// タイム数字空白
	const D3DXVECTOR3 SPACE_TIME_PART	= D3DXVECTOR3(SIZE_TIME_PART.x * 0.85f, 0.0f, 0.0f);	// タイム区切り空白

	const float	SET_TIME_SCALE	= 8.0f;	// タイム表示の初期拡大率
	const float	SUB_TIME_SCALE	= 0.3f;	// タイム表示拡大率の減算量
	const int	TIME_WAIT_CNT	= 3;	// タイム表示状態への変更待機フレーム数

	const D3DXVECTOR3 POS_CONT_LOGO		= D3DXVECTOR3(SCREEN_CENT.x, 490.0f, 0.0f);	// コンテニューロゴ位置
	const D3DXVECTOR3 SIZE_CONT_LOGO	= D3DXVECTOR3(576.0f, 172.0f, 0.0f);		// コンテニューロゴ大きさ
	const D3DXVECTOR3 POS_CONT_YES		= D3DXVECTOR3(400.0f, 610.0f, 0.0f);		// コンテニュー表示のYESの位置
	const D3DXVECTOR3 POS_CONT_NO		= D3DXVECTOR3(880.0f, 610.0f, 0.0f);		// コンテニュー表示のNOの位置
	const D3DXVECTOR3 SIZE_CONT			= D3DXVECTOR3(222.0f, 94.0f, 0.0f);			// コンテニュー表示の大きさ

	const float	SET_CONT_SCALE	= 8.0f;	// コンテニュー表示の初期拡大率
	const float	SUB_CONT_SCALE	= 0.3f;	// コンテニュー表示拡大率の減算量
	const int	CONT_WAIT_CNT	= 3;	// コンテニュー表示状態への変更待機フレーム数

	const D3DXCOLOR CHOICE_COL	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 選択中カラー
	const D3DXCOLOR DEFAULT_COL	= D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);	// 非選択中カラー
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CResultManager::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	親クラス [CResultManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CResultManager::CResultManager() :
	m_pContLogo		(nullptr),		// コンテニューロゴの情報
	m_pTimeLogo		(nullptr),		// タイムロゴの情報
	m_pFade			(nullptr),		// フェードの情報
	m_pTime			(nullptr),		// タイムの情報
	m_state			(STATE_NONE),	// 状態
	m_nSelect		(SELECT_YES),	// 現在の選択
	m_nOldSelect	(SELECT_YES),	// 前回の選択
	m_fScale		(0.0f),			// ポリゴン拡大率
	m_nCounterState	(0)				// 状態管理カウンター
{
	// メンバ変数をクリア
	memset(&m_apResult[0], 0, sizeof(m_apResult));		// リザルト表示の情報
	memset(&m_apContinue[0], 0, sizeof(m_apContinue));	// コンテニュー表示の情報
}

//============================================================
//	デストラクタ
//============================================================
CResultManager::~CResultManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CResultManager::Init(void)
{
	// 変数配列を宣言
	static D3DXVECTOR3 aPosResult[] =	// リザルトの位置
	{
		POS_RESULT_MISSION,	// MISSION位置
		POS_RESULT_RESULT,	// RESULT位置
	};
	static D3DXVECTOR3 aPosContinue[] =	// コンテニューの位置
	{
		POS_CONT_YES,	// YES位置
		POS_CONT_NO	,	// NO位置
	};

	// ポインタを宣言
	CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	memset(&m_apResult[0], 0, sizeof(m_apResult));		// リザルト表示の情報
	memset(&m_apContinue[0], 0, sizeof(m_apContinue));	// コンテニュー表示の情報
	m_pContLogo		= nullptr;		// コンテニューロゴの情報
	m_pTimeLogo		= nullptr;		// タイムロゴの情報
	m_pFade			= nullptr;		// フェードの情報
	m_pTime			= nullptr;		// タイムの情報
	m_state			= STATE_FADEIN;	// 状態
	m_nCounterState	= 0;			// 状態管理カウンター
	m_nSelect		= SELECT_YES;	// 現在の選択
	m_nOldSelect	= SELECT_YES;	// 前回の選択
	m_fScale		= 0.0f;			// ポリゴン拡大率

	//--------------------------------------------------------
	//	フェードの生成・設定
	//--------------------------------------------------------
	// フェードの生成
	m_pFade = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		SIZE_FADE,		// 大きさ
		VEC3_ZERO,		// 向き
		INITCOL_FADE	// 色
	);
	if (m_pFade == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pFade->SetPriority(PRIORITY);

	//--------------------------------------------------------
	//	リザルト表示の生成・設定
	//--------------------------------------------------------
	for (int nCntResult = 0; nCntResult < result::NUM_POLYGON; nCntResult++)
	{ // リザルト表示の総数分繰り返す

		// リザルト表示の生成
		m_apResult[nCntResult] = CObject2D::Create
		( // 引数
			aPosResult[nCntResult],			// 位置
			SIZE_RESULT * SET_RESULT_SCALE	// 大きさ
		);
		if (m_apResult[nCntResult] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_apResult[nCntResult]->SetPriority(PRIORITY);

		// 描画をしない設定にする
		m_apResult[nCntResult]->SetEnableDraw(false);
	}

	// リザルト表示のテクスチャを設定
	SetTexResult();

	//--------------------------------------------------------
	//	タイムロゴ表示の生成・設定
	//--------------------------------------------------------
	// タイムロゴ表示の生成
	m_pTimeLogo = CObject2D::Create
	( // 引数
		POS_TIME_LOGO,					// 位置
		SIZE_TIME_LOGO * SET_TIME_SCALE	// 大きさ
	);
	if (m_pTimeLogo == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pTimeLogo->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_TIME]));

	// 優先順位を設定
	m_pTimeLogo->SetPriority(PRIORITY);

	// 描画をしない設定にする
	m_pTimeLogo->SetEnableDraw(false);

	//--------------------------------------------------------
	//	タイム表示の生成・設定
	//--------------------------------------------------------
	// タイムUIの生成
	m_pTime = CTimeUI::Create
	( // 引数
		0.0f,								// 制限時間
		POS_TIME,							// 位置
		SIZE_TIME_VAL * SET_TIME_SCALE,		// 数字の大きさ
		SIZE_TIME_PART * SET_TIME_SCALE,	// 区切りの大きさ
		SPACE_TIME_VAL,						// 数字の空白
		SPACE_TIME_PART						// 区切りの空白
	);
	if (m_pTime == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pTime->SetPriority(PRIORITY);

	// 描画をしない設定にする
	m_pTime->SetEnableDraw(false);

	// タイムを設定
	m_pTime->SetTime(GET_RETENTION->GetTime());

	//--------------------------------------------------------
	//	コンテニューロゴ表示の生成・設定
	//--------------------------------------------------------
	// コンテニューロゴ表示の生成
	m_pContLogo = CObject2D::Create
	( // 引数
		POS_CONT_LOGO,					// 位置
		SIZE_CONT_LOGO * SET_CONT_SCALE	// 大きさ
	);
	if (m_pContLogo == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pContLogo->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_CONTINUE]));

	// 優先順位を設定
	m_pContLogo->SetPriority(PRIORITY);

	// 描画をしない設定にする
	m_pContLogo->SetEnableDraw(false);

	//--------------------------------------------------------
	//	コンテニュー表示の生成・設定
	//--------------------------------------------------------
	for (int nCntResult = 0; nCntResult < SELECT_MAX; nCntResult++)
	{ // 選択肢の総数分繰り返す

		// コンテニュー表示の生成
		m_apContinue[nCntResult] = CObject2D::Create
		( // 引数
			aPosContinue[nCntResult],	// 位置
			SIZE_CONT * SET_CONT_SCALE	// 大きさ
		);
		if (m_apContinue[nCntResult] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_apContinue[nCntResult]->SetPriority(PRIORITY);

		// 描画をしない設定にする
		m_apContinue[nCntResult]->SetEnableDraw(false);

		// 色を設定
		m_apContinue[nCntResult]->SetColor(DEFAULT_COL);
	}

	// テクスチャを登録・割当
	m_apContinue[SELECT_YES]->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_YES]));
	m_apContinue[SELECT_NO]->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_NO]));

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CResultManager::Uninit(void)
{
	// タイムの終了
	SAFE_UNINIT(m_pTime);

	for (int nCntResult = 0; nCntResult < result::NUM_POLYGON; nCntResult++)
	{ // リザルト表示の総数分繰り返す

		// リザルト表示の終了
		SAFE_UNINIT(m_apResult[nCntResult]);
	}

	for (int nCntResult = 0; nCntResult < SELECT_MAX; nCntResult++)
	{ // 選択肢の総数分繰り返す

		// コンテニュー表示の終了
		SAFE_UNINIT(m_apContinue[nCntResult]);
	}

	// コンテニューロゴ表示の終了
	SAFE_UNINIT(m_pContLogo);

	// タイムロゴ表示の終了
	SAFE_UNINIT(m_pTimeLogo);

	// フェードの終了
	SAFE_UNINIT(m_pFade);
}

//============================================================
//	更新処理
//============================================================
void CResultManager::Update(const float fDeltaTime)
{
	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:	// 何もしない状態

		// 無し

		break;

	case STATE_FADEIN:	// フェードイン状態

		// フェードインの更新
		UpdateFade();

		break;

	case STATE_RESULT:	// リザルト表示状態

		// リザルト表示の更新
		UpdateResult();

		break;

	case STATE_TIME_WAIT:	// タイム表示待機状態

		// 表示待機の更新
		if (UpdateDrawWait(TIME_WAIT_CNT))
		{ // 待機完了の場合

			// タイム表示の拡大率を設定
			m_fScale = SET_TIME_SCALE;

			// タイム表示の描画開始
			m_pTimeLogo->SetEnableDraw(true);
			m_pTime->SetEnableDraw(true);

			// 状態を変更
			m_state = STATE_TIME;	// タイム表示状態
		}

		break;

	case STATE_TIME:	// タイム表示状態

		// タイム表示の更新
		UpdateTime();

		break;

	case STATE_CONTINUE_WAIT:	// コンテニュー表示待機状態

		// 表示待機の更新
		if (UpdateDrawWait(CONT_WAIT_CNT))
		{ // 待機完了の場合

			// コンテニュー表示の拡大率を設定
			m_fScale = SET_CONT_SCALE;

			// コンテニューロゴ表示の描画開始
			m_pContLogo->SetEnableDraw(true);
			
			for (int nCntResult = 0; nCntResult < SELECT_MAX; nCntResult++)
			{ // 選択肢の総数分繰り返す

				// コンテニュー表示の描画開始
				m_apContinue[nCntResult]->SetEnableDraw(true);
			}

			// 状態を変更
			m_state = STATE_CONTINUE;	// コンテニュー表示状態
		}

		break;

	case STATE_CONTINUE:	// コンテニュー表示待機状態表示状態

		// コンテニュー表示の更新
		UpdateContinue();

		break;

	case STATE_WAIT:	// 遷移待機状態

		// 選択の更新
		UpdateSelect();

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 遷移決定の更新
	UpdateTransition();

	for (int nCntResult = 0; nCntResult < result::NUM_POLYGON; nCntResult++)
	{ // リザルト表示の総数分繰り返す

		// リザルト表示の更新
		m_apResult[nCntResult]->Update(fDeltaTime);
	}

	for (int nCntResult = 0; nCntResult < SELECT_MAX; nCntResult++)
	{ // 選択肢の総数分繰り返す

		// コンテニュー表示の更新
		m_apContinue[nCntResult]->Update(fDeltaTime);
	}

	// コンテニューロゴ表示の更新
	m_pContLogo->Update(fDeltaTime);

	// タイムロゴ表示の更新
	m_pTimeLogo->Update(fDeltaTime);

	// タイムの更新
	m_pTime->Update(fDeltaTime);

	// フェードの更新
	m_pFade->Update(fDeltaTime);
}

//============================================================
//	生成処理
//============================================================
CResultManager *CResultManager::Create(void)
{
	// リザルトマネージャーの生成
	CResultManager *pResultManager = new CResultManager;
	if (pResultManager == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// リザルトマネージャーの初期化
		if (FAILED(pResultManager->Init()))
		{ // 初期化に失敗した場合

			// リザルトマネージャーの破棄
			SAFE_DELETE(pResultManager);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pResultManager;
	}
}

//============================================================
//	破棄処理
//============================================================
void CResultManager::Release(CResultManager *&prResultManager)
{
	// リザルトマネージャーの終了
	assert(prResultManager != nullptr);
	prResultManager->Uninit();

	// メモリ開放
	SAFE_DELETE(prResultManager);
}

//============================================================
//	フェードインの更新処理
//============================================================
void CResultManager::UpdateFade(void)
{
	// 変数を宣言
	D3DXCOLOR colFade = m_pFade->GetColor();	// フェードの色

	if (colFade.a < SETCOL_FADE.a)
	{ // 透明量が設定値未満の場合

		// 透明度を加算
		colFade.a += ADD_ALPHA;
	}
	else
	{ // 透明量が設定値以上の場合

		// 透明度を補正
		colFade.a = SETCOL_FADE.a;

		for (int nCntResult = 0; nCntResult < result::NUM_POLYGON; nCntResult++)
		{ // リザルト表示の総数分繰り返す

			// リザルト表示の描画開始
			m_apResult[nCntResult]->SetEnableDraw(true);
		}

		// リザルト表示の拡大率を設定
		m_fScale = SET_RESULT_SCALE;

		// 状態を変更
		m_state = STATE_RESULT;	// リザルト表示状態
	}

	// 透明度を反映
	m_pFade->SetColor(colFade);
}

//============================================================
//	リザルト表示の更新処理
//============================================================
void CResultManager::UpdateResult(void)
{
	if (m_fScale > 1.0f)
	{ // 拡大率が最小値より大きい場合

		// 拡大率を減算
		m_fScale -= SUB_RESULT_SCALE;

		for (int nCntResult = 0; nCntResult < result::NUM_POLYGON; nCntResult++)
		{ // リザルト表示の総数分繰り返す

			// リザルト表示の大きさを設定
			m_apResult[nCntResult]->SetVec3Sizing(SIZE_RESULT * m_fScale);
		}
	}
	else
	{ // 拡大率が最小値以下の場合

		for (int nCntResult = 0; nCntResult < result::NUM_POLYGON; nCntResult++)
		{ // リザルト表示の総数分繰り返す

			// リザルト表示の大きさを設定
			m_apResult[nCntResult]->SetVec3Sizing(SIZE_RESULT);
		}

		switch (GET_RETENTION->GetWin())
		{ // リザルトごとの処理
		case CRetentionManager::WIN_FAILED:

			// 状態を変更
			m_state = STATE_CONTINUE_WAIT;	// コンテニュー表示待機状態

			break;

		case CRetentionManager::WIN_CLEAR:

			// 状態を変更
			m_state = STATE_TIME_WAIT;	// タイム表示待機状態

			break;

		default:

			// エラーメッセージボックス
			MessageBox(nullptr, "リザルトなしが設定されています", "警告！", MB_ICONWARNING);

			// 状態を変更
			m_state = STATE_CONTINUE_WAIT;	// コンテニュー表示待機状態

			break;
		}

		// サウンドの再生
		PLAY_SOUND(CSound::LABEL_SE_DECISION_001);	// 決定音01
	}
}

//============================================================
//	タイム表示処理
//============================================================
void CResultManager::UpdateTime(void)
{
	if (m_fScale > 1.0f)
	{ // 拡大率が最小値より大きい場合

		// 拡大率を減算
		m_fScale -= SUB_TIME_SCALE;

		// タイム表示の大きさを設定
		m_pTimeLogo->SetVec3Sizing(SIZE_TIME_LOGO * m_fScale);
		m_pTime->SetSizingValue(SIZE_TIME_VAL * m_fScale);
		m_pTime->SetSizingPart(SIZE_TIME_PART * m_fScale);
	}
	else
	{ // 拡大率が最小値以下の場合

		// 拡大率を補正
		m_fScale = 1.0f;

		// タイム表示の大きさを設定
		m_pTimeLogo->SetVec3Sizing(SIZE_TIME_LOGO);
		m_pTime->SetSizingValue(SIZE_TIME_VAL);
		m_pTime->SetSizingPart(SIZE_TIME_PART);

		// 状態を変更
		m_state = STATE_CONTINUE_WAIT;	// コンテニュー表示待機状態

		// サウンドの再生
		PLAY_SOUND(CSound::LABEL_SE_DECISION_001);	// 決定音01
	}
}

//============================================================
//	コンテニュー表示処理
//============================================================
void CResultManager::UpdateContinue(void)
{
	if (m_fScale > 1.0f)
	{ // 拡大率が最小値より大きい場合

		// 拡大率を減算
		m_fScale -= SUB_CONT_SCALE;

		// コンテニューロゴ表示の大きさを設定
		m_pContLogo->SetVec3Sizing(SIZE_CONT_LOGO * m_fScale);

		for (int nCntResult = 0; nCntResult < SELECT_MAX; nCntResult++)
		{ // 選択肢の総数分繰り返す

			// コンテニュー表示の大きさを設定
			m_apContinue[nCntResult]->SetVec3Sizing(SIZE_CONT * m_fScale);
		}
	}
	else
	{ // 拡大率が最小値以下の場合

		// 拡大率を補正
		m_fScale = 1.0f;

		// コンテニューロゴ表示の大きさを設定
		m_pContLogo->SetVec3Sizing(SIZE_CONT_LOGO);

		for (int nCntResult = 0; nCntResult < SELECT_MAX; nCntResult++)
		{ // 選択肢の総数分繰り返す

			// コンテニュー表示の大きさを設定
			m_apContinue[nCntResult]->SetVec3Sizing(SIZE_CONT);
		}

		// 状態を変更
		m_state = STATE_WAIT;	// 遷移待機状態

		// サウンドの再生
		PLAY_SOUND(CSound::LABEL_SE_DECISION_001);	// 決定音01
	}
}

//============================================================
//	選択の更新処理
//============================================================
void CResultManager::UpdateSelect(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= GET_INPUTKEY;	// キーボード
	CInputPad		*pPad		= GET_INPUTPAD;		// パッド

	if (pKeyboard->IsTrigger(DIK_A)
	||  pKeyboard->IsTrigger(DIK_LEFT)
	||  pPad->IsTrigger(CInputPad::KEY_LEFT))
	{ // 左移動の操作が行われた場合

		// 左に選択をずらす
		m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX;

		// サウンドの再生
		PLAY_SOUND(CSound::LABEL_SE_SELECT_000);	// 選択操作音00
	}
	if (pKeyboard->IsTrigger(DIK_D)
	||  pKeyboard->IsTrigger(DIK_RIGHT)
	||  pPad->IsTrigger(CInputPad::KEY_RIGHT))
	{ // 右移動の操作が行われた場合

		// 右に選択をずらす
		m_nSelect = (m_nSelect + 1) % SELECT_MAX;

		// サウンドの再生
		PLAY_SOUND(CSound::LABEL_SE_SELECT_000);	// 選択操作音00
	}

	// 前回の選択要素の色を黒に設定
	m_apContinue[m_nOldSelect]->SetColor(DEFAULT_COL);

	// 現在の選択要素の色を白に設定
	m_apContinue[m_nSelect]->SetColor(CHOICE_COL);

	// 現在の選択要素を代入
	m_nOldSelect = m_nSelect;
}

//============================================================
//	遷移決定の更新処理
//============================================================
void CResultManager::UpdateTransition(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= GET_INPUTKEY;	// キーボード
	CInputPad		*pPad		= GET_INPUTPAD;		// パッド

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
			PLAY_SOUND(CSound::LABEL_SE_DECISION_001);	// 決定音01
		}
		else
		{ // 遷移待機状態の場合

			if (GET_MANAGER->GetFade()->GetState() == CFade::FADE_NONE)
			{ // フェード中ではない場合

				switch (m_nSelect)
				{ // 選択ごとの処理
				case SELECT_YES:

					// ゲーム画面に遷移
					GET_MANAGER->SetFadeScene(CScene::MODE_GAME);

					break;

				case SELECT_NO:

					// ランキング画面に遷移
					GET_MANAGER->SetFadeScene(CScene::MODE_RANKING);

					break;
				}

				// サウンドの再生
				PLAY_SOUND(CSound::LABEL_SE_DECISION_000);	// 決定音00
			}
		}
	}
}

//============================================================
//	演出スキップ処理
//============================================================
void CResultManager::SkipStaging(void)
{
	// リザルト表示の描画をONにし、大きさを設定
	for (int nCntResult = 0; nCntResult < result::NUM_POLYGON; nCntResult++)
	{ // リザルト表示の総数分繰り返す

		// リザルト表示の描画開始
		m_apResult[nCntResult]->SetEnableDraw(true);

		// リザルト表示の大きさを設定
		m_apResult[nCntResult]->SetVec3Sizing(SIZE_RESULT);
	}

	if (GET_RETENTION->GetWin() == CRetentionManager::WIN_CLEAR)
	{ // クリアしている場合

		// タイム表示をONにする
		m_pTimeLogo->SetEnableDraw(true);
		m_pTime->SetEnableDraw(true);

		// タイム表示の大きさを設定
		m_pTimeLogo->SetVec3Sizing(SIZE_TIME_LOGO);
		m_pTime->SetSizingValue(SIZE_TIME_VAL);
		m_pTime->SetSizingPart(SIZE_TIME_PART);
	}

	// コンテニューロゴ表示の描画開始
	m_pContLogo->SetEnableDraw(true);

	// コンテニューロゴ表示の大きさを設定
	m_pContLogo->SetVec3Sizing(SIZE_CONT_LOGO);

	for (int nCntResult = 0; nCntResult < SELECT_MAX; nCntResult++)
	{ // 選択肢の総数分繰り返す

		// コンテニュー表示の描画開始
		m_apContinue[nCntResult]->SetEnableDraw(true);

		// コンテニュー表示の大きさを設定
		m_apContinue[nCntResult]->SetVec3Sizing(SIZE_CONT);
	}

	// フェードの透明度を設定
	m_pFade->SetColor(SETCOL_FADE);

	// 状態を変更
	m_state = STATE_WAIT;	// 遷移待機状態
}

//============================================================
//	リザルト表示のテクスチャの設定処理
//============================================================
void CResultManager::SetTexResult(void)
{
	// ポインタを宣言
	CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ

	// MISSIONテクスチャを登録・割当
	m_apResult[0]->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_MISSION]));

	// RESULTテクスチャを登録・割当
	switch (GET_RETENTION->GetWin())
	{ // リザルトごとの処理
	case CRetentionManager::WIN_FAILED:

		// FAILEDテクスチャ
		m_apResult[1]->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_FAILED]));

		break;

	case CRetentionManager::WIN_CLEAR:

		// CLEARテクスチャ
		m_apResult[1]->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_CLEAR]));

		break;

	default:

		// エラーメッセージボックス
		MessageBox(nullptr, "リザルトなしが設定されています", "警告！", MB_ICONWARNING);

		// FAILEDテクスチャ
		m_apResult[1]->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_FAILED]));

		break;
	}
}

//============================================================
//	表示待機処理
//============================================================
bool CResultManager::UpdateDrawWait(const int nWait)
{
	if (m_nCounterState < nWait)
	{ // カウンターが待機カウントまで達していない場合

		// カウンターを加算
		m_nCounterState++;

		// 待機未完了を返す
		return false;
	}
	else
	{ // カウンターが待機完了した場合

		// カウンターを初期化
		m_nCounterState = 0;

		// 待機完了を返す
		return true;
	}
}
