//============================================================
//
//	デバッグヘッダー [debug.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _DEBUG_H_
#define _DEBUG_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "camera.h"

//************************************************************
//	クラス定義
//************************************************************
// デバッグクラス
class CDebug
{
public:
	// コンストラクタ
	CDebug();

	// デストラクタ
	~CDebug();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void MeasureFps(const DWORD dwCurrentTime);	// FPS測定
	void AddFrameCount(void);		// フレームカウント加算
	int  GetFps(void) const;		// FPS取得
	void SetFillMode(void);			// 塗りつぶしモード設定
	void UpdateDebugControl(void);	// デバッグ操作更新
	void DrawDebugControl(void);	// デバッグ操作表示
	void DrawDebugData(void);		// デバッグ情報表示

	// 静的メンバ関数
	static CDebug *Create(void);			// 生成
	static void Release(CDebug *&pDebug);	// 破棄

private:

#ifdef _DEBUG

	// メンバ関数
	void ChangeDispDebug(void);		// デバッグ表示変更
	void ChangeControlCamera(void);	// カメラ操作変更
	void ChangeFillMode(void);		// 塗りつぶしモード変更
	void ChangeDisp2D(void);		// 2Dオブジェクト表示変更
	void ChangeEditMode(void);		// エディターモード変更
	void ChangeDispPause(void);		// ポーズ表示変更
	void ResultTrans(void);			// リザルト遷移

	// メンバ変数
	int  m_nFps;	// FPSカウンタ
	bool m_bDisp2D;	// 2D表示状況
	bool m_bDisp3D;	// 3D表示状況
	DWORD m_dwFrameCount;	// フレームカウント
	DWORD m_dwFPSLastTime;	// 最後にFPSを計測した時刻
	D3DFILLMODE m_fillMode;	// 塗りつぶしモード
	CCamera::EState m_oldCameraState;	// カメラの過去状態

#endif	// _DEBUG

};

#endif	// _DEBUG_H_
