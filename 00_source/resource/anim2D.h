//============================================================
//
//	アニメーション2Dヘッダー [anim2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ANIM2D_H_
#define _ANIM2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object2D.h"

//************************************************************
//	クラス定義
//************************************************************
// アニメーション2Dクラス
class CAnim2D : public CObject2D
{
public:
	// コンストラクタ
	explicit CAnim2D(const CObject::ELabel label = LABEL_NONE, const EDim dimension = DIM_2D, const int nPriority = object::DEFAULT_PRIO);

	// デストラクタ
	~CAnim2D() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;			// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	void SetVec3Rotation(const D3DXVECTOR3& rRot) override;	// 向き設定
	void SetVec3Sizing(const D3DXVECTOR3& rSize) override;	// 大きさ設定

	// 静的メンバ関数
	static CAnim2D *Create	// 生成
	( // 引数
		const int nWidthPtrn,		// テクスチャの横の分割数
		const int nHeightPtrn,		// テクスチャの縦の分割数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rSize = VEC3_ONE,	// 大きさ
		const D3DXVECTOR3& rRot = VEC3_ZERO,	// 向き
		const D3DXCOLOR& rCol = XCOL_WHITE		// 色
	);

	// メンバ関数
	void SetColor(const D3DXCOLOR& rCol);			// 色設定
	void SetPattern(const int nPattern);			// パターンの設定
	void SetMaxPattern(const int nMaxPtrn);			// パターンの総数の設定
	void SetWidthPattern(const int nWidthPtrn);		// テクスチャの横分割数の設定
	void SetHeightPattern(const int nHeightPtrn);	// テクスチャの縦分割数の設定
	void SetCounter(const int nCntChange);			// カウンターの設定
	void SetEnableStop(const bool bStop);			// 停止状況の設定
	void SetEnablePlayBack(const bool bPlayBack);	// 逆再生状況の設定

	int GetPattern(void) const			{ return m_nPattern; }	// パターンの取得
	int GetLoopAnimation(void) const	{ return m_nNumLoop; }	// パターン繰り返し数の取得

private:
	// メンバ変数
	int m_nCounter;		// アニメーションカウンター
	int m_nCntChange;	// パターン変更カウント
	int m_nPattern;		// アニメーションパターン
	int m_nMaxPtrn;		// パターンの総数
	int m_nWidthPtrn;	// テクスチャの横の分割数
	int m_nHeightPtrn;	// テクスチャの縦の分割数
	int m_nNumLoop;		// パターン繰り返し数
	bool m_bStop;		// 停止状況
	bool m_bPlayBack;	// 逆再生状況
};

#endif	// _ANIM2D_H_
