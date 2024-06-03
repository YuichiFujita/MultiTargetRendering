//============================================================
//
//	ブラーヘッダー [blur.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _BLUR_H_
#define _BLUR_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "motion.h"

//************************************************************
//	インクルードファイル
//************************************************************
class CObjectModel;	// オブジェクトモデルクラス

//************************************************************
//	クラス定義
//************************************************************
// ブラークラス
class CBlur : public CObject
{
public:
	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,	// 何もしない状態
		STATE_NORMAL,	// 通常状態
		STATE_VANISH,	// 消失状態
		STATE_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CBlur();

	// デストラクタ
	~CBlur();

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CBlur *Create	// 生成
	( // 引数
		CObjectChara *pParent,		// 親オブジェクト
		D3DXMATERIAL& rMat,			// ブラーマテリアル
		const float fStartAlpha,	// ブラー開始透明度
		const int nMaxLength		// 保持オブジェクト最大数
	);

	// メンバ関数
	void SetState(const EState state);	// 状態設定
	EState GetState(void) const					{ return m_state; }	// 状態取得
	void SetMaterial(const D3DXMATERIAL& rMat)	{ m_mat = rMat; }	// ブラーマテリアル設定
	D3DXMATERIAL GetMaterial(void) const		{ return m_mat; }	// ブラーマテリアル取得
	void SetStartAlpha(const float fStartAlpha)	{ m_fStartAlpha = fStartAlpha; }	// ブラーの開始透明度設定
	float GetStartAlpha(void) const				{ return m_fStartAlpha; }			// ブラーの開始透明度取得
	void SetMaxLength(const int nMaxLength)		{ m_nMaxLength = nMaxLength; }		// 保持オブジェクト数設定
	int GetMaxLength(void) const				{ return m_nMaxLength; }			// 保持オブジェクト数取得

private:
	// オーバーライド関数
	void Release(void) override { CObject::Release(); }	// 破棄

	// メンバ変数
	std::vector<std::vector<CObjectModel*>> m_oldObject;	// 過去オブジェクト情報
	CObjectChara *m_pParent;	// 親オブジェクト
	D3DXMATERIAL m_mat;			// ブラー反映マテリアル
	float	m_fStartAlpha;		// ブラーの開始透明度
	int		m_nMaxLength;		// 保持する親オブジェクトの最大数
	EState	m_state;			// 状態
	int		m_nCounterState;	// 状態管理カウンター

};

#endif	// _BLUR_H_
