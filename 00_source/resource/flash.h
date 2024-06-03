//============================================================
//
//	フラッシュヘッダー [flash.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _FLASH_H_
#define _FLASH_H_

//************************************************************
//	前方宣言
//************************************************************
class CObject2D;	// オブジェクト2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// フラッシュクラス
class CFlash
{
public:
	// フラッシュ状態列挙
	enum EFlash
	{
		FLASH_NONE = 0,	// 何もしていない状態
		FLASH_IN,		// フェードイン状態
		FLASH_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CFlash();

	// デストラクタ
	~CFlash();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(const float fDeltaTime);	// 更新

	void Set(const float fStartAlpha, const float fSubAlpha);	// フラッシュ設定
	EFlash GetState(void) const { return m_flash; }				// フラッシュ状態取得

	// 静的メンバ関数
	static CFlash *Create(void);			// 生成
	static void Release(CFlash *&pFlash);	// 破棄

private:
	// メンバ変数
	CObject2D *m_pObject2D;	// フラッシュ表示の情報
	EFlash m_flash;			// フラッシュ状態
	float m_fSubAlpha;		// 透明度の減算量
};

#endif	// _FLASH_H_
