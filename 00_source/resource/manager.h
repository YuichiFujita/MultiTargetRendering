//============================================================
//
//	マネージャーヘッダー [manager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MANAGER_H_
#define _MANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "scene.h"

//************************************************************
//	前方宣言
//************************************************************
class CDeltaTime;			// デルタタイムクラス
class CRenderer;			// レンダラークラス
class CInputKeyboard;		// キーボードクラス
class CInputMouse;			// マウスクラス
class CInputPad;			// パッドクラス
class CSound;				// サウンドクラス
class CCamera;				// カメラクラス
class CLight;				// ライトクラス
class CTexture;				// テクスチャクラス
class CModel;				// モデルクラス
class CFont;				// フォントクラス
class CCharacter;			// キャラクタークラス
class CFade;				// フェードクラス
class CLoading;				// ローディングクラス
class CRetentionManager;	// データ保存マネージャークラス
class CDebugProc;			// デバッグ表示クラス
class CDebug;				// デバッグクラス

//************************************************************
//	クラス定義
//************************************************************
// マネージャークラス
class CManager
{
public:
	// コンストラクタ
	CManager();

	// デストラクタ
	~CManager();

	// メンバ関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);	// 初期化
	HRESULT Load(void);	// 読込
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CManager *Create(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);	// 生成
	static CManager *GetInstance(void);			// 取得
	static void Release(CManager *&prManager);	// 破棄
	static void ReleaseWindow(void);			// ウインドウ破棄

	// メンバ関数
	void SetFadeScene(const CScene::EMode mode, const int nWait = 0);	// シーン設定 (フェード：ON)
	HRESULT SetScene(const CScene::EMode mode);		// シーン設定 (フェード：OFF)
	HRESULT InitScene(const CScene::EMode mode);	// シーン初期化
	HRESULT SetMode(const CScene::EMode mode);		// モード設定
	CScene::EMode GetMode(void);					// モード取得

	CDeltaTime			*GetDeltaTime(void);	// デルタタイム取得
	CRenderer			*GetRenderer(void);		// レンダラー取得
	CInputKeyboard		*GetKeyboard(void);		// キーボード取得
	CInputMouse			*GetMouse(void);		// マウス取得
	CInputPad			*GetPad(void);			// パッド取得
	CSound				*GetSound(void);		// サウンド取得
	CCamera				*GetCamera(void);		// カメラ取得
	CLight				*GetLight(void);		// ライト取得
	CTexture			*GetTexture(void);		// テクスチャ取得
	CModel				*GetModel(void);		// モデル取得
	CFont				*GetFont(void);			// フォント取得
	CCharacter			*GetCharacter(void);	// キャラクター取得
	CFade				*GetFade(void);			// フェード取得
	CLoading			*GetLoading(void);		// ローディング取得
	CScene				*GetScene(void);		// シーン取得
	CRetentionManager	*GetRetention(void);	// データ保存マネージャー取得
	CDebugProc			*GetDebugProc(void);	// デバッグ表示取得
	CDebug				*GetDebug(void);		// デバッグ取得

private:
	// 静的メンバ変数
	static CManager		*m_pManager;	// マネージャー

	// メンバ変数
	HINSTANCE			m_hInstance;	// インスタンスハンドル
	HWND				m_hWnd;			// ウインドウハンドル
	CDeltaTime			*m_pDeltaTime;	// デルタタイムインスタンス
	CRenderer			*m_pRenderer;	// レンダラーインスタンス
	CInputKeyboard		*m_pKeyboard;	// キーボードインスタンス
	CInputMouse			*m_pMouse;		// マウスインスタンス
	CInputPad			*m_pPad;		// パッドインスタンス
	CSound				*m_pSound;		// サウンドインスタンス
	CCamera				*m_pCamera;		// カメラインスタンス
	CLight				*m_pLight;		// ライトインスタンス
	CTexture			*m_pTexture;	// テクスチャインスタンス
	CModel				*m_pModel;		// モデルインスタンス
	CFont				*m_pFont;		// フォントインスタンス
	CCharacter			*m_pCharacter;	// キャラクターインスタンス
	CFade				*m_pFade;		// フェードインスタンス
	CLoading			*m_pLoading;	// ローディングインスタンス
	CScene				*m_pScene;		// シーンインスタンス
	CRetentionManager	*m_pRetention;	// データ保存マネージャー
	CDebugProc			*m_pDebugProc;	// デバッグ表示
	CDebug				*m_pDebug;		// デバッグ
};

#endif	// _MANAGER_H_
