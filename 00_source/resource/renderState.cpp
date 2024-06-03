//============================================================
//
//	レンダーステート処理 [renderState.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "renderState.h"
#include "manager.h"
#include "renderer.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const CRenderState::SBlendAlpha SET_BLEND[] =	// 列挙ごとのαブレンド
	{
		RS_BL_NORMAL,	// 通常αブレンド
		RS_BL_ADD,		// 加算αブレンド
		RS_BL_SUB,		// 減算αブレンド
	};
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(SET_BLEND) == CRenderState::BLEND_MAX, "ERROR : Blend Count Mismatch");

//************************************************************
//	静的メンバ変数宣言
//************************************************************
LPDIRECT3DDEVICE9 *CRenderState::m_ppDevice = nullptr;	// Direct3Dデバイスへのポインタ
CRenderState::SInfo CRenderState::m_save = {};			// 保存レンダーステート情報

//************************************************************
//	親クラス [CRenderState] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CRenderState::CRenderState()
{
	// メンバ変数をクリア
	memset(&m_info, 0, sizeof(m_info));	// レンダーステート情報
}

//============================================================
//	デストラクタ
//============================================================
CRenderState::~CRenderState()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CRenderState::Init(void)
{
	// メンバ変数を初期化
	m_info = m_save;	// レンダーステート情報

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CRenderState::Uninit(void)
{

}

//============================================================
//	設定処理
//============================================================
void CRenderState::Set(void)
{
	// 設定中のレンダーステートを設定
	SetRenderState(m_info);
}

//============================================================
//	再設定処理
//============================================================
void CRenderState::Reset(void)
{
	// 保存済みのレンダーステートに戻す
	SetRenderState(m_save);
}

//============================================================
//	レンダーステート情報の設定処理
//============================================================
void CRenderState::SetInfoRenderState(const SInfo & rInfo)
{
	// 引数のレンダーステート情報を設定
	m_info = rInfo;
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState::SInfo CRenderState::GetInfoRenderState(void)
{
	// レンダーステート情報を返す
	return m_info;
}

//============================================================
//	αブレンド情報の設定処理
//============================================================
void CRenderState::SetAlphaBlend(const EBlend blend)
{
	if (blend > NONE_IDX && blend < BLEND_MAX)
	{ // ブレンド種類の範囲内の場合

		// 引数のαブレンドを設定
		m_info.blendAlpha = SET_BLEND[blend];
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	生成処理
//============================================================
CRenderState *CRenderState::Create(void)
{
	// レンダーステートの生成
	CRenderState *pRenderState = new CRenderState;
	if (pRenderState == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// レンダーステートの初期化
		if (FAILED(pRenderState->Init()))
		{ // 初期化に失敗した場合

			// レンダーステートの破棄
			SAFE_DELETE(pRenderState);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pRenderState;
	}
}

//============================================================
//	破棄処理
//============================================================
void CRenderState::Release(CRenderState *&prRenderState)
{
	// レンダーステートの終了
	assert(prRenderState != nullptr);
	prRenderState->Uninit();

	// メモリ開放
	SAFE_DELETE(prRenderState);
}

//============================================================
//	デバイスの割当処理
//============================================================
void CRenderState::BindDevice(LPDIRECT3DDEVICE9 *ppDevice)
{
	// デバイス未指定
	assert(*ppDevice != nullptr);

	// デバイスのアドレスを保存
	m_ppDevice = ppDevice;
}

//============================================================
//	レンダーステート情報の初期化処理
//============================================================
void CRenderState::InitRenderState(void)
{
	// αブレンド情報を初期化
	m_save.blendAlpha.op		= D3DBLENDOP_ADD;
	m_save.blendAlpha.scr		= D3DBLEND_SRCALPHA;
	m_save.blendAlpha.dest		= D3DBLEND_INVSRCALPHA;
	m_save.blendAlpha.bBlend	= true;

	// αテスト情報を初期化
	m_save.testAlpha.func	= D3DCMP_ALWAYS;
	m_save.testAlpha.nRef	= 0;
	m_save.testAlpha.bTest	= false;

	// Zテスト情報を初期化
	m_save.testZ.func		= D3DCMP_LESSEQUAL;
	m_save.testZ.bUpdate	= true;

	// カリング情報を初期化
	m_save.cull = D3DCULL_CCW;

	// ライティング状況を初期化
	m_save.bLight = true;

	// レンダーステートの初期化
	SetRenderState(m_save);
}

//============================================================
//	保存レンダーステート情報の取得処理
//============================================================
CRenderState::SInfo CRenderState::GetSaveRenderState(void)
{
	// 保存レンダーステート情報を返す
	return m_save;
}

//============================================================
//	レンダーステートの設定処理
//============================================================
void CRenderState::SetRenderState(const SInfo& rInfo)
{
	// デバイス未設定
	assert(*m_ppDevice != nullptr);

	// αブレンディングを設定
	(*m_ppDevice)->SetRenderState(D3DRS_ALPHABLENDENABLE, rInfo.blendAlpha.bBlend);
	(*m_ppDevice)->SetRenderState(D3DRS_BLENDOP, rInfo.blendAlpha.op);
	(*m_ppDevice)->SetRenderState(D3DRS_SRCBLEND, rInfo.blendAlpha.scr);
	(*m_ppDevice)->SetRenderState(D3DRS_DESTBLEND, rInfo.blendAlpha.dest);

	// αテストを設定
	(*m_ppDevice)->SetRenderState(D3DRS_ALPHAFUNC, rInfo.testAlpha.func);			// αテストの設定
	(*m_ppDevice)->SetRenderState(D3DRS_ALPHAREF, rInfo.testAlpha.nRef);			// αテストの参照値設定
	(*m_ppDevice)->SetRenderState(D3DRS_ALPHATESTENABLE, rInfo.testAlpha.bTest);	// αテストの有効 / 無効の設定

	// Zテストを設定
	(*m_ppDevice)->SetRenderState(D3DRS_ZFUNC, rInfo.testZ.func);			// Zテストの設定
	(*m_ppDevice)->SetRenderState(D3DRS_ZWRITEENABLE, rInfo.testZ.bUpdate);	// Zバッファ更新の有効 / 無効の設定

	// ポリゴンの表示状態を設定
	(*m_ppDevice)->SetRenderState(D3DRS_CULLMODE, rInfo.cull);

	// ライティングを設定
	(*m_ppDevice)->SetRenderState(D3DRS_LIGHTING, rInfo.bLight);
}
