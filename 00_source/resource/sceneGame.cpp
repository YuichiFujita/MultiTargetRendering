//============================================================
//
//	ゲーム画面処理 [sceneGame.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "sceneGame.h"
#include "manager.h"
#include "sound.h"
#include "camera.h"

#include "gameManager.h"
#include "timerUI.h"
#include "cinemaScope.h"
#include "pause.h"
#include "hitStop.h"
#include "flash.h"
#include "stage.h"
#include "player.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	namespace timerInfo
	{
		const float TIME_START = 0.0f;		// 開始時間
		const float TIME_LIMIT = 180.0f;	// 制限時間
		const D3DXVECTOR3 POS		 = D3DXVECTOR3(1100.0f, 43.5f, 0.0f);	// タイマー位置
		const D3DXVECTOR3 OFFSET	 = D3DXVECTOR3(146.0f, 13.0f, 0.0f);	// タイマー背景オフセット
		const D3DXVECTOR3 VAL_SIZE	 = D3DXVECTOR3(46.8f, 62.4f, 0.0f);		// タイマー数字大きさ
		const D3DXVECTOR3 PART_SIZE	 = D3DXVECTOR3(27.3f, 62.4f, 0.0f);		// タイマー区切り大きさ
		const D3DXVECTOR3 BG_SIZE	 = D3DXVECTOR3(397.5f, 33.5f, 0.0f);	// タイマー背景大きさ
		const D3DXVECTOR3 VAL_SPACE	 = D3DXVECTOR3(VAL_SIZE.x * 0.85f, 0.0f, 0.0f);		// タイマー数字空白
		const D3DXVECTOR3 PART_SPACE = D3DXVECTOR3(PART_SIZE.x * 0.85f, 0.0f, 0.0f);	// タイマー区切り空白
	}
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CGameManager	*CSceneGame::m_pGameManager	= nullptr;	// ゲームマネージャー
CTimerUI		*CSceneGame::m_pTimerUI		= nullptr;	// タイマーUI
CCinemaScope	*CSceneGame::m_pCinemaScope	= nullptr;	// シネマスコープ
CPause			*CSceneGame::m_pPause		= nullptr;	// ポーズ
CHitStop		*CSceneGame::m_pHitStop		= nullptr;	// ヒットストップ
CFlash			*CSceneGame::m_pFlash		= nullptr;	// フラッシュ

//************************************************************
//	子クラス [CSceneGame] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSceneGame::CSceneGame(const EMode mode) : CScene(mode)
{

}

//============================================================
//	デストラクタ
//============================================================
CSceneGame::~CSceneGame()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSceneGame::Init(void)
{
	//--------------------------------------------------------
	//	初期生成
	//--------------------------------------------------------
	// シーンの初期化
	CScene::Init();	// ステージ・プレイヤーの生成

	// タイマーUIの生成
	m_pTimerUI = CTimerUI::Create
	( // 引数
		timerInfo::TIME_START,	// 開始時間
		timerInfo::TIME_LIMIT,	// 制限時間
		timerInfo::POS,			// 位置
		timerInfo::VAL_SIZE,	// 数字の大きさ
		timerInfo::PART_SIZE,	// 区切りの大きさ
		timerInfo::VAL_SPACE,	// 数字の空白
		timerInfo::PART_SPACE	// 区切りの空白
	);
	if (m_pTimerUI == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// シネマスコープの生成
	m_pCinemaScope = CCinemaScope::Create();
	if (m_pCinemaScope == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ポーズの生成
	m_pPause = CPause::Create();
	if (m_pPause == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ヒットストップの生成
	m_pHitStop = CHitStop::Create();
	if (m_pHitStop == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// フラッシュの生成
	m_pFlash = CFlash::Create();
	if (m_pFlash == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ゲームマネージャーの生成
	m_pGameManager = CGameManager::Create();
	if (m_pGameManager == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	初期設定
	//--------------------------------------------------------
	// タイマーの計測を開始する
	m_pTimerUI->Start();

	// BGMの再生
	PLAY_SOUND(CSound::LABEL_BGM_GAME);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CSceneGame::Uninit(void)
{
	// ゲームマネージャーの破棄
	SAFE_REF_RELEASE(m_pGameManager);

	// タイマーUIの終了
	SAFE_UNINIT(m_pTimerUI);

	// シネマスコープの破棄
	SAFE_REF_RELEASE(m_pCinemaScope);

	// ポーズの破棄
	SAFE_REF_RELEASE(m_pPause);

	// ヒットストップの破棄
	SAFE_REF_RELEASE(m_pHitStop);

	// フラッシュの破棄
	SAFE_REF_RELEASE(m_pFlash);

	// シーンの終了
	CScene::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CSceneGame::Update(const float fDeltaTime)
{
	// ゲームマネージャーの更新
	assert(m_pGameManager != nullptr);
	m_pGameManager->Update(fDeltaTime);

	// ヒットストップの更新
	assert(m_pHitStop != nullptr);
	m_pHitStop->Update(fDeltaTime);

	// フラッシュの更新
	assert(m_pFlash != nullptr);
	m_pFlash->Update(fDeltaTime);

	if (m_pGameManager->GetState() == CGameManager::STATE_NORMAL && !m_pHitStop->IsStop())
	{ // ゲームが通常状態の場合

		// ポーズの更新
		assert(m_pPause != nullptr);
		m_pPause->Update(fDeltaTime);
	}

	if (!m_pPause->IsPause()
	&&  !m_pHitStop->IsStop())
	{ // ポーズ中・ヒットストップ中ではない場合

		// シネマスコープの更新
		assert(m_pCinemaScope != nullptr);
		m_pCinemaScope->Update(fDeltaTime);

		// シーンの更新
		CScene::Update(fDeltaTime);
	}
	else if (m_pHitStop->IsStop())
	{ // ヒットストップ中の場合

		// シネマスコープの更新
		assert(m_pCinemaScope != nullptr);
		m_pCinemaScope->Update(fDeltaTime);

		// カメラの更新
		GET_MANAGER->GetCamera()->Update(fDeltaTime);
	}

#ifdef _DEBUG

	else
	{ // ポーズ中の場合

		if (GET_MANAGER->GetCamera()->GetState() == CCamera::STATE_CONTROL)
		{ // カメラが操作状態の場合

			// カメラの更新
			GET_MANAGER->GetCamera()->Update(fDeltaTime);
		}
	}

#endif	// _DEBUG
}

//============================================================
//	ゲームマネージャー取得処理
//============================================================
CGameManager *CSceneGame::GetGameManager(void)
{
	// インスタンス未使用
	assert(m_pGameManager != nullptr);

	// ゲームマネージャーのポインタを返す
	return m_pGameManager;
}

//============================================================
//	タイマーUI取得処理
//============================================================
CTimerUI *CSceneGame::GetTimerUI(void)
{
	// インスタンス未使用
	assert(m_pTimerUI != nullptr);

	// タイマーUIのポインタを返す
	return m_pTimerUI;
}

//============================================================
//	シネマスコープ取得処理
//============================================================
CCinemaScope *CSceneGame::GetCinemaScope(void)
{
	// インスタンス未使用
	assert(m_pCinemaScope != nullptr);

	// シネマスコープのポインタを返す
	return m_pCinemaScope;
}

//============================================================
//	ポーズ取得処理
//============================================================
CPause *CSceneGame::GetPause(void)
{
	// インスタンス未使用
	assert(m_pPause != nullptr);

	// ポーズのポインタを返す
	return m_pPause;
}

//============================================================
//	ヒットストップ取得処理
//============================================================
CHitStop *CSceneGame::GetHitStop(void)
{
	// インスタンス未使用
	assert(m_pHitStop != nullptr);

	// ヒットストップのポインタを返す
	return m_pHitStop;
}

//============================================================
//	フラッシュ取得処理
//============================================================
CFlash *CSceneGame::GetFlash(void)
{
	// インスタンス未使用
	assert(m_pFlash != nullptr);

	// フラッシュのポインタを返す
	return m_pFlash;
}
