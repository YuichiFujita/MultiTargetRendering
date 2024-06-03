//============================================================
//
//	波動ヘッダー [wave.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _WAVE_H_
#define _WAVE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "scrollMeshRing.h"

//************************************************************
//	定数宣言
//************************************************************
namespace ring
{
	const POSGRID2 PART		= POSGRID2(16, 1);	// 分割数
	const POSGRID2 TEX_PART	= POSGRID2(16, 1);	// テクスチャ分割数
	const float HOLE_RADIUS	= 200.0f;	// 穴の半径
	const float THICKNESS	= 80.0f;	// 太さ
	const float OUTER_PLUSY	= 0.0f;		// 外周のY座標加算量
}

//************************************************************
//	クラス定義
//************************************************************
// 波動クラス
class CWave : public CScrollMeshRing
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_NONE = 0,	// テクスチャなし
		TEXTURE_SWORD,		// 剣の波動
		TEXTURE_IMPACT,		// 衝撃波の波動
		TEXTURE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CWave();

	// デストラクタ
	~CWave() override;

	// 成長構造体
	struct SGrow
	{
		// コンストラクタ
		SGrow() {}
		SGrow(const float fRadius, const float fThick, const float fAlpha)
		{ fAddHoleRadius = fRadius; fAddThickness = fThick; fSubAlpha = fAlpha; }

		// デストラクタ
		~SGrow() {}

		// メンバ変数
		float fAddHoleRadius;	// 穴の半径の成長量
		float fAddThickness;	// 太さの成長量
		float fSubAlpha;		// 透明度の成長量
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CWave *Create	// 生成
	( // 引数
		const ETexture texture,		// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXCOLOR& rCol,		// 色
		const SGrow& rGrow,			// 成長量
		const POSGRID2& rPart		= ring::PART,			// 分割数
		const POSGRID2& rTexPart	= ring::TEX_PART,		// テクスチャ分割数
		const float fHoleRadius		= ring::HOLE_RADIUS,	// 穴の半径
		const float fThickness		= ring::THICKNESS,		// 太さ
		const float fOuterPlusY		= ring::OUTER_PLUSY		// 外周のY座標加算量
	);

	// メンバ関数
	void SetTexture(const ETexture texture);	// テクスチャ設定
	void SetGrow(const SGrow& rGrow);			// 成長情報設定
	SGrow GetGrow(void) const;					// 成長情報取得

private:
	// メンバ変数
	SGrow m_grow;	// 成長の情報
};

#endif	// _WAVE_H_
