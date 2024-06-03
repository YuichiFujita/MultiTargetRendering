//============================================================
//
//	ローディング処理 [loading.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "loading.h"
#include "manager.h"
#include "renderer.h"
#include "object.h"
#include "anim2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char* TEXTURE_FILE = "data\\TEXTURE\\loading000.tga";	// ロード画面テクスチャ

	const POSGRID2 PTRN = POSGRID2(12, 4);	// テクスチャ分割数
	const int	PRIORITY	= 7;			// ローディングの優先順位
	const int	ANIM_WAIT	= 2;			// アニメーションの変更フレーム
	const float	FADE_LEVEL	= 0.05f;		// フェードのα値の加減量

#ifdef _DEBUG

	const int MIN_LOAD = 0;	// 最低ロード繰り返し数

#else	// NDEBUG

	const int MIN_LOAD = 2;	// 最低ロード繰り返し数

#endif	// _DEBUG
}

//************************************************************
//	親クラス [CLoading] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CLoading::CLoading() :
	m_pPromise	(nullptr),		// プロミス
	m_pFuture	(nullptr),		// フューチャー
	m_pLoad		(nullptr),		// ロード画面情報
	m_state		(LOAD_NONE),	// ロード状態
	m_bEnd		(false)			// ロード終了状況
{

}

//============================================================
//	デストラクタ
//============================================================
CLoading::~CLoading()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CLoading::Init(void)
{
	// メンバ変数を初期化
	m_pPromise	= nullptr;		// プロミス
	m_pFuture	= nullptr;		// フューチャー
	m_pLoad		= nullptr;		// ロード画面情報
	m_state		= LOAD_NONE;	// ロード状態
	m_bEnd		= false;		// ロード終了状況

	// ロード画面の生成
	m_pLoad = CAnim2D::Create
	( // 引数
		PTRN.x,			// テクスチャ横分割数
		PTRN.y,			// テクスチャ縦分割数
		SCREEN_CENT,	// 位置
		SCREEN_SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		XCOL_AWHITE		// 色
	);
	if (m_pLoad == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pLoad->BindTexture(TEXTURE_FILE);

	// 優先順位の設定
	m_pLoad->SetPriority(PRIORITY);

	// ラベル指定なしに設定
	m_pLoad->SetLabel(CObject::LABEL_NONE);

	// 変更フレームの設定
	m_pLoad->SetCounter(ANIM_WAIT);

	// ロードのアニメーションを停止
	m_pLoad->SetEnableStop(true);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CLoading::Uninit(void)
{
	if (m_func.joinable())
	{ // 読込関数の処理が終了していないい場合

		// 処理終了を待機
		m_func.join();
		
		// ここを消すとメインスレッドで
		// 情報の破棄後に読込を行いメモリ破壊する
	}

	// プロミスオブジェクトの破棄
	SAFE_DELETE(m_pPromise);

	// フューチャーオブジェクトの破棄
	SAFE_DELETE(m_pFuture);

	// ロード画面の終了
	SAFE_UNINIT(m_pLoad);
}

//============================================================
//	更新処理
//============================================================
void CLoading::Update(const float fDeltaTime)
{
	if (m_state != LOAD_NONE)
	{ // 何もしない状態ではない場合

		D3DXCOLOR colLoad = m_pLoad->GetColor();	// フェード色
		switch (m_state)
		{ // ロード状態ごとの処理
		case LOAD_FADEOUT:	// ロードの表示開始状態

			// α値を加算
			colLoad.a += FADE_LEVEL;
			if (colLoad.a >= 1.0f)
			{ // α値が 1.0f を上回った場合

				// α値を補正
				colLoad.a = 1.0f;

				// ロードのアニメーションを再生
				m_pLoad->SetEnableStop(false);

				// ロード更新状態にする
				m_state = LOAD_UPDATE;
			}

			break;

		case LOAD_UPDATE:	// ロード更新状態

			if (m_bEnd)
			{ // 初期化処理が終了した場合

				if (SUCCEEDED(m_pFuture->get()))
				{ // 関数の処理が成功した場合

					// ロード待機状態にする
					m_state = LOAD_WAIT;

					// プロミスオブジェクトの破棄
					SAFE_DELETE(m_pPromise);

					// フューチャーオブジェクトの破棄
					SAFE_DELETE(m_pFuture);
				}
				else
				{ // 関数の処理が失敗した場合

					// エラーメッセージボックス
					MessageBox(nullptr, "ローディングに失敗！", "警告！", MB_ICONWARNING);

					// ウインドウを破棄する
					CManager::ReleaseWindow();
				}
			}

			break;

		case LOAD_WAIT:		// ロード待機状態

#ifdef NDEBUG
			// ロードのキリが悪い場合抜ける
			if (m_pLoad->GetPattern() != 0) { break; }
#endif	// NDEBUG

			// 最低ロード回数未満の場合抜ける
			if (m_pLoad->GetLoopAnimation() < MIN_LOAD) { break; }

			// ロードのアニメーションを停止
			m_pLoad->SetEnableStop(true);

			// 関数情報を切り離す
			m_func.detach();

			// ロードの表示終了状態にする
			m_state = LOAD_FADEIN;

			break;

		case LOAD_FADEIN:	// ロードの表示終了状態

			// α値を減算
			colLoad.a -= FADE_LEVEL;
			if (colLoad.a <= 0.0f)
			{ // α値が 0.0fを下回った場合

				// α値を補正
				colLoad.a = 0.0f;

				// 何もしない状態にする
				m_state = LOAD_NONE;
			}

			break;

		default:	// 例外処理
			assert(false);
			break;
		}

		// 色の更新
		m_pLoad->SetColor(colLoad);

		// ロード画面の更新
		m_pLoad->Update(fDeltaTime);
	}
}

//============================================================
//	描画処理
//============================================================
void CLoading::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// サンプラーステートを設定
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);	// U方向のラッピングを無効化
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);	// V方向のラッピングを無効化

	// ロード画面の描画
	m_pLoad->Draw();

	// サンプラーステートを設定
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// U方向のラッピングを有効化
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// V方向のラッピングを有効化
}

//============================================================
//	ロード開始設定処理
//============================================================
HRESULT CLoading::Set(std::function<HRESULT(bool*)> func)
{
	if (m_state == LOAD_NONE)
	{ // ローディングが行われていない場合

		// ロード未完了にする
		m_bEnd = false;

		if (m_pPromise == nullptr)
		{
			// プロミスの生成
			m_pPromise = new std::promise<HRESULT>;
		}
		else { assert(false); return E_FAIL; }

		if (m_pFuture == nullptr)
		{
			// フューチャーの生成
			m_pFuture = new std::future<HRESULT>;
		}
		else { assert(false); return E_FAIL; }

		// 生成したプロミスからフューチャーを取得
		*m_pFuture = m_pPromise->get_future();

		// 返り値の取得できるラムダ関数を作成
		auto funcLambda = [func, this](bool *pEnd)
		{
			// 引数の関数を呼び出し
			HRESULT hr = func(pEnd);

			// 返り値をプロミスに格納
			m_pPromise->set_value(hr);
		};

		// スレッドに引数の関数を設定
		m_func = std::thread(funcLambda, &m_bEnd);

		// ロード画面のパターンを先頭にする
		m_pLoad->SetPattern(0);

		// ロードの表示開始状態にする
		m_state = LOAD_FADEOUT;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// ローディングできない場合エラー
}

//============================================================
//	生成処理
//============================================================
CLoading *CLoading::Create(void)
{
	// ローディングの生成
	CLoading *pLoading = new CLoading;
	if (pLoading == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// ローディングの初期化
		if (FAILED(pLoading->Init()))
		{ // 初期化に失敗した場合

			// ローディングの破棄
			SAFE_DELETE(pLoading);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pLoading;
	}
}

//============================================================
//	破棄処理
//============================================================
void CLoading::Release(CLoading *&prLoading)
{
	// ローディングの終了
	assert(prLoading != nullptr);
	prLoading->Uninit();

	// メモリ開放
	SAFE_DELETE(prLoading);
}
