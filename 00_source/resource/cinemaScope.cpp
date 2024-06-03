//============================================================
//
//	シネマスコープ処理 [cinemaScope.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "cinemaScope.h"
#include "renderer.h"
#include "manager.h"
#include "sceneGame.h"
#include "sound.h"
#include "fade.h"
#include "texture.h"
#include "object2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 5;	// シネマスコープ表示の優先順位

	namespace scope
	{
		const D3DXVECTOR3	SIZE	= D3DXVECTOR3(SCREEN_SIZE.x, 94.0f, 0.0f);	// スコープの大きさ
		const D3DXVECTOR3	POS		= D3DXVECTOR3(SCREEN_SIZE.x + SIZE.x * 0.5f, SIZE.y * 0.5f, 0.0f);	// スコープの位置
		const D3DXVECTOR3	SPACE	= D3DXVECTOR3(-SIZE.x * 2.0f, SCREEN_SIZE.y - SIZE.y, 0.0f);		// スコープの空白
		const D3DXCOLOR		COL		= XCOL_BLACK;	// スコープの色

		const D3DXVECTOR3	STOP_POS	= D3DXVECTOR3(SCREEN_CENT.x, SIZE.y * 0.5f, 0.0f);	// スコープ停止の位置
		const D3DXVECTOR3	STOP_SPACE	= D3DXVECTOR3(0.0f, SCREEN_SIZE.y - SIZE.y, 0.0f);	// スコープ停止の空白

		const D3DXVECTOR3	END_POS		= D3DXVECTOR3(-SIZE.x * 0.5f, SIZE.y * 0.5f, 0.0f);			// スコープ終了の位置
		const D3DXVECTOR3	END_SPACE	= D3DXVECTOR3(SIZE.x * 2.0f, SCREEN_SIZE.y - SIZE.y, 0.0f);	// スコープ終了の空白

		const int SCOPE_FRAME = 65;	// スコープ移動フレーム
	}
}

//************************************************************
//	子クラス [CCinemaScope] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CCinemaScope::CCinemaScope() :
	m_state			(STATE_NONE),	// 状態
	m_nCounterScope	(0)				// スコープ移動管理カウンター
{
	// メンバ変数をクリア
	memset(&m_apBrack[0], 0, sizeof(m_apBrack));	// 黒ポリゴンの情報
}

//============================================================
//	デストラクタ
//============================================================
CCinemaScope::~CCinemaScope()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CCinemaScope::Init(void)
{
	// メンバ変数を初期化
	memset(&m_apBrack[0], 0, sizeof(m_apBrack));	// 黒ポリゴンの情報
	m_state = STATE_NONE;	// 状態
	m_nCounterScope = 0;	// スコープ移動管理カウンター

	for (int nCntScope = 0; nCntScope < POLYGON_MAX; nCntScope++)
	{ // ポリゴン数分繰り返す

		// 黒ポリゴンの生成
		m_apBrack[nCntScope] = CObject2D::Create
		( // 引数
			scope::POS + (scope::SPACE * (float)nCntScope),	// 位置
			scope::SIZE,	// 大きさ
			VEC3_ZERO,		// 向き
			scope::COL		// 色
		);
		if (m_apBrack[nCntScope] == nullptr)
		{ // 非使用中の場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_apBrack[nCntScope]->SetPriority(PRIORITY);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CCinemaScope::Uninit(void)
{
	for (int nCntScope = 0; nCntScope < POLYGON_MAX; nCntScope++)
	{ // ポリゴン数分繰り返す

		// 黒ポリゴンの終了
		SAFE_UNINIT(m_apBrack[nCntScope]);
	}
}

//============================================================
//	更新処理
//============================================================
void CCinemaScope::Update(const float fDeltaTime)
{
	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:
		break;

	case STATE_SCOPE_IN:

		// スコープインの更新
		UpdateScopeIn();

		break;

	case STATE_SCOPE_OUT:

		// スコープアウトの更新
		UpdateScopeOut();

		break;

	default:
		assert(false);
		break;
	}

	for (int nCntScope = 0; nCntScope < POLYGON_MAX; nCntScope++)
	{ // ポリゴン数分繰り返す

		// 黒ポリゴンの更新
		m_apBrack[nCntScope]->Update(fDeltaTime);
	}
}

//============================================================
//	描画処理
//============================================================
void CCinemaScope::Draw(void)
{

}

//============================================================
//	スコープインの設定処理
//============================================================
void CCinemaScope::SetScopeIn(void)
{
	if (m_state == STATE_SCOPE_IN) { return; }	// スコープインの場合抜ける

	// カウンターを初期化
	m_nCounterScope = 0;

	// スコープイン状態にする
	m_state = STATE_SCOPE_IN;
}

//============================================================
//	スコープアウトの設定処理
//============================================================
void CCinemaScope::SetScopeOut(void)
{
	if (m_state == STATE_SCOPE_OUT) { return; }	// スコープアウトの場合抜ける

	// カウンターを初期化
	m_nCounterScope = 0;

	// スコープアウト状態にする
	m_state = STATE_SCOPE_OUT;
}

//============================================================
//	描画状況の設定処理
//============================================================
void CCinemaScope::SetEnableDraw(const bool bDraw)
{
	for (int nCntScope = 0; nCntScope < POLYGON_MAX; nCntScope++)
	{ // ポリゴン数分繰り返す

		// 黒ポリゴンの描画状況を設定
		m_apBrack[nCntScope]->SetEnableDraw(bDraw);
	}
}

//============================================================
//	生成処理
//============================================================
CCinemaScope *CCinemaScope::Create(void)
{
	// シネマスコープの生成
	CCinemaScope *pCinemaScope = new CCinemaScope;
	if (pCinemaScope == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// シネマスコープの初期化
		if (FAILED(pCinemaScope->Init()))
		{ // 初期化に失敗した場合

			// シネマスコープの破棄
			SAFE_DELETE(pCinemaScope);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pCinemaScope;
	}
}

//============================================================
//	破棄処理
//============================================================
void CCinemaScope::Release(CCinemaScope *&prCinemaScope)
{
	// シネマスコープの終了
	assert(prCinemaScope != nullptr);
	prCinemaScope->Uninit();

	// メモリ開放
	SAFE_DELETE(prCinemaScope);
}

//============================================================
//	スコープインの更新処理
//============================================================
void CCinemaScope::UpdateScopeIn(void)
{
	// カウンターを加算
	m_nCounterScope++;
	if (m_nCounterScope <= scope::SCOPE_FRAME)
	{ // スコープ停止前の場合

		for (int nCntScope = 0; nCntScope < POLYGON_MAX; nCntScope++)
		{ // ポリゴン数分繰り返す

			D3DXVECTOR3 posPoly = m_apBrack[nCntScope]->GetVec3Position();					// ポリゴン位置
			float fStartPos = scope::POS.x + (scope::SPACE.x * (float)nCntScope);			// 動作開始位置
			float fStopPos = scope::STOP_POS.x + (scope::STOP_SPACE.x * (float)nCntScope);	// 動作停止位置

			float fRate = easeing::InOutQuad(m_nCounterScope, 0, scope::SCOPE_FRAME);	// 現在フレームのイージング値取得
			float fPosX = (fStopPos - fStartPos) * fRate;								// X座標を求める

			// 位置を反映
			posPoly.x = fStartPos + fPosX;	// 現在のX座標を更新
			m_apBrack[nCntScope]->SetVec3Position(posPoly);
		}
	}
	else
	{ // スコープ停止後の場合

		// カウンターを初期化
		m_nCounterScope = 0;

		// なにもしない状態にする
		m_state = STATE_NONE;
	}
}

//============================================================
//	スコープアウトの更新処理
//============================================================
void CCinemaScope::UpdateScopeOut(void)
{
	// カウンターを加算
	m_nCounterScope++;
	if (m_nCounterScope <= scope::SCOPE_FRAME)
	{ // スコープ終了前の場合

		for (int nCntScope = 0; nCntScope < POLYGON_MAX; nCntScope++)
		{ // ポリゴン数分繰り返す

			D3DXVECTOR3 posPoly = m_apBrack[nCntScope]->GetVec3Position();					// ポリゴン位置
			float fStopPos = scope::STOP_POS.x + (scope::STOP_SPACE.x * (float)nCntScope);	// 動作開始位置
			float fEndPos = scope::END_POS.x + (scope::END_SPACE.x * (float)nCntScope);		// 動作停止位置

			float fRate = easeing::InOutQuad(m_nCounterScope, 0, scope::SCOPE_FRAME);	// 現在フレームのイージング値取得
			float fPosX = (fEndPos - fStopPos) * fRate;									// X座標を求める

			// 位置を反映
			posPoly.x = fStopPos + fPosX;	// 現在のX座標を更新
			m_apBrack[nCntScope]->SetVec3Position(posPoly);
		}
	}
	else
	{ // スコープ終了後の場合

		// カウンターを初期化
		m_nCounterScope = 0;

		// なにもしない状態にする
		m_state = STATE_NONE;
	}
}
