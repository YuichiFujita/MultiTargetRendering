//============================================================
//
//	デルタタイムヘッダー [deltaTime.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _DELTATIME_H_
#define _DELTATIME_H_

//************************************************************
//	クラス定義
//************************************************************
// デルタタイムクラス
class CDeltaTime
{
public:
	// コンストラクタ
	CDeltaTime();

	// デストラクタ
	~CDeltaTime();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	float GetTime(void) { return m_fTime; }	// 経過時間取得

	// 静的メンバ関数
	static CDeltaTime *Create(void);				// 生成
	static void Release(CDeltaTime *&prDeltaTime);	// 破棄

private:
	// メンバ変数
	DWORD m_dwOldTime;	// 前回の処理開始時刻
	DWORD m_dwCurTime;	// 今回の処理開始時刻
	float m_fTime;		// 処理の経過時間
};

#endif	// _DELTATIME_H_
