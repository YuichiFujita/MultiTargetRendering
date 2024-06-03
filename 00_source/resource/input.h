//============================================================
//
//	入力ヘッダー [input.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _INPUT_H_
#define _INPUT_H_

//************************************************************
//	定数宣言
//************************************************************
namespace keyboard
{
	const int MAX_KEY = 256;	// キーの最大数
}

namespace pad
{
	const int	MAX_NUM		= 4;	// パッドの最大数
	const float	DEAD_ZONE	= (float)USHRT_MAX * 0.1f;	// スティックの無視する傾き量
}

//************************************************************
//	クラス定義
//************************************************************
// 入力クラス
class CInput
{
public:
	// コンストラクタ
	CInput();

	// デストラクタ
	virtual ~CInput();

	// 仮想関数
	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);	// 初期化
	virtual void Uninit(void);	// 終了

	// 純粋仮想関数
	virtual void Update(void) = 0;	// 更新

protected:
	// メンバ変数
	static int m_nNumAll;				// 入力デバイスの総数
	static LPDIRECTINPUT8 m_pInput;		// DirectInputオブジェクト
	LPDIRECTINPUTDEVICE8  m_pDevice;	// DirectInputデバイス
};

// キーボードクラス
class CInputKeyboard : public CInput
{
public:
	// コンストラクタ
	CInputKeyboard();

	// デストラクタ
	~CInputKeyboard();

	// オーバーライド関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd) override;	// 初期化
	void Uninit(void) override;	// 終了
	void Update(void) override;	// 更新

	// 静的メンバ関数
	static CInputKeyboard *Create(HINSTANCE hInstance, HWND hWnd);	// 生成
	static void Release(CInputKeyboard *&pKeyboard);				// 破棄

	// メンバ関数
	bool IsPress(int nKey);		// プレス取得
	bool IsTrigger(int nKey);	// トリガー取得
	bool IsRelease(int nKey);	// リリース取得
	bool IsAnyPress(void);		// 全プレス取得
	bool IsAnyTrigger(void);	// 全トリガー取得
	bool IsAnyRelease(void);	// 全リリース取得

private:
	// メンバ変数
	BYTE m_aKeyStatePress[keyboard::MAX_KEY];	// プレス情報
	BYTE m_aKeyStateTrigger[keyboard::MAX_KEY];	// トリガー情報
	BYTE m_aKeyStateRelease[keyboard::MAX_KEY];	// リリース情報
};

// マウスクラス
class CInputMouse : public CInput
{
public:
	// コンストラクタ
	CInputMouse();

	// デストラクタ
	~CInputMouse();

	// マウスキー列挙
	enum EKey
	{
		KEY_LEFT = 0,	// マウス右クリック
		KEY_RIGHT,		// マウス左クリック
		KEY_WHEEL,		// マウスホイール押し込み
		KEY_MAX			// この列挙型の総数
	};

	// オーバーライド関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd) override;	// 初期化
	void Uninit(void) override;	// 終了
	void Update(void) override;	// 更新

	// 静的メンバ関数
	static CInputMouse *Create(HINSTANCE hInstance, HWND hWnd);	// 生成
	static void Release(CInputMouse *&pMouse);					// 破棄

	// メンバ関数
	bool IsPress(EKey mouKey);		// プレス取得
	bool IsTrigger(EKey mouKey);	// トリガー取得
	bool IsRelease(EKey mouKey);	// リリース取得
	bool IsAnyPress(void);			// 全プレス取得
	bool IsAnyTrigger(void);		// 全トリガー取得
	bool IsAnyRelease(void);		// 全リリース取得
	D3DXVECTOR3 GetMove(void);		// 移動量取得

private:
	// メンバ変数
	DIMOUSESTATE m_keyStatePress;	// プレス情報
	DIMOUSESTATE m_keyStateTrigger;	// トリガー情報
	DIMOUSESTATE m_keyStateRelease;	// リリース情報
};

// パッドクラス
class CInputPad
{
public:
	// コンストラクタ
	CInputPad();

	// デストラクタ
	~CInputPad();

	// パッドキー列挙
	enum EKey
	{
		KEY_UP = 0,		// 方向キー 上
		KEY_DOWN,		// 方向キー 下
		KEY_LEFT,		// 方向キー 左
		KEY_RIGHT,		// 方向キー 右
		KEY_START,		// STARTボタン
		KEY_BACK,		// BACKボタン
		KEY_LSTICKPUSH,	// 左スティック押し込み
		KEY_RSTICKPUSH,	// 右スティック押し込み
		KEY_LB,			// LBボタン
		KEY_RB,			// RBボタン
		KEY_LT,			// LTボタン
		KEY_RT,			// RTボタン
		KEY_A,			// Aボタン
		KEY_B,			// Bボタン
		KEY_X,			// Xボタン
		KEY_Y,			// Yボタン
		KEY_LSTICK_X,	// 左スティック 左右
		KEY_LSTICK_Y,	// 左スティック 上下
		KEY_RSTICK_X,	// 右スティック 左右
		KEY_RSTICK_Y,	// 右スティック 上下
		KEY_MAX			// この列挙型の総数
	};
	
	// 振動種類列挙
	enum EType
	{
		TYPE_NONE = 0,		// 何もしていない
		TYPE_DAMAGE,		// ダメージ
		TYPE_BIG_DAMAGE,	// 大ダメージ
		TYPE_DEATH,			// 死亡
		TYPE_WALLDASH,		// 壁走り
		TYPE_MAX			// この列挙型の総数
	};

	// 振動構造体
	struct SVibration
	{
		_XINPUT_VIBRATION vibration;	// バイブ情報
		EType type;	// バイブ種類
		int nTime;	// バイブ時間
	};

	// 静的メンバ関数
	static CInputPad *Create(void);			// 生成
	static void Release(CInputPad *&pPad);	// 破棄

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetVibration(EType type, int nPadID = 0);	// バイブ設定
	bool IsPress(EKey joyKey, int nPadID = 0);		// プレス取得	(ボタン)
	bool IsTrigger(EKey joyKey, int nPadID = 0);	// トリガー取得	(ボタン)
	bool IsRelease(EKey joyKey, int nPadID = 0);	// リリース取得	(ボタン)

	bool IsPressAll(EKey joyKey);		// 全プレス取得		(ボタン)
	bool IsTriggerAll(EKey joyKey);		// 全トリガー取得	(ボタン)
	bool IsReleaseAll(EKey joyKey);		// 全リリース取得	(ボタン)
	bool IsAnyPress(int nPadID = 0);	// 全プレス取得		(ボタン)
	bool IsAnyTrigger(int nPadID = 0);	// 全トリガー取得	(ボタン)
	bool IsAnyRelease(int nPadID = 0);	// 全リリース取得	(ボタン)

	void InitPress(int nPadID = 0);		// プレス初期化		(ボタン)
	void InitTrigger(int nPadID = 0);	// トリガー初期化	(ボタン)
	void InitRelease(int nPadID = 0);	// リリース初期化	(ボタン)

	SHORT GetPressLStickX(int nPadID = 0);		// プレス取得 (LスティックX)
	SHORT GetPressLStickY(int nPadID = 0);		// プレス取得 (LスティックY)
	SHORT GetPressRStickX(int nPadID = 0);		// プレス取得 (RスティックX)
	SHORT GetPressRStickY(int nPadID = 0);		// プレス取得 (RスティックY)
	float GetPressLStickRot(int nPadID = 0);	// プレス取得 (Lスティック向き)
	float GetPressRStickRot(int nPadID = 0);	// プレス取得 (Rスティック向き)
	float GetPressLStickTilt(int nPadID = 0);	// プレス取得 (Lスティック傾き)
	float GetPressRStickTilt(int nPadID = 0);	// プレス取得 (Rスティック傾き)

private:
	// メンバ関数
	void UpdateVibration(SVibration *pVibration, int nPadID);	// バイブレーション更新

	// メンバ変数
	SVibration   m_aVibration[pad::MAX_NUM];		// バイブ情報
	XINPUT_STATE m_aKeyStatePress[pad::MAX_NUM];	// プレス情報
	XINPUT_STATE m_aKeyStateTrigger[pad::MAX_NUM];	// トリガー情報
	XINPUT_STATE m_aKeyStateRelease[pad::MAX_NUM];	// リリース情報
};

#endif	// _INPUT_H_
