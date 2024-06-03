//============================================================
//
//	エフェクト2Dヘッダー [effect2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EFFECT2D_H_
#define _EFFECT2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object2D.h"
#include "renderState.h"

//************************************************************
//	クラス定義
//************************************************************
// エフェクト2Dクラス
class CEffect2D : public CObject2D
{
public:
	// 種類列挙
	enum EType
	{
		TYPE_NORMAL = 0,	// 通常テクスチャ
		TYPE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	explicit CEffect2D(const EType type, const CObject::ELabel label = LABEL_NONE);

	// デストラクタ
	~CEffect2D() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// メンバ関数
	void SetAlphaBlend(const CRenderState::EBlend blend);	// αブレンド設定

	// 静的メンバ関数
	static CEffect2D *Create	// 生成
	( // 引数
		const D3DXVECTOR3&	rPos,		// 位置
		const float			fRadius,	// 半径
		const EType			type		= TYPE_NORMAL,	// テクスチャ
		const int			nLife		= 10,			// 寿命
		const D3DXVECTOR3&	rMove		= VEC3_ZERO,	// 移動量
		const D3DXVECTOR3&	rRot		= VEC3_ZERO,	// 向き
		const D3DXCOLOR&	rCol		= XCOL_WHITE,	// 色
		const float			fSubSize	= 0.0f,			// 半径の減算量
		const CRenderState::EBlend	blend	= CRenderState::BLEND_ADD,	// αブレンド状況
		const CObject::ELabel		label	= LABEL_EFFECT				// オブジェクトラベル
	);

private:
	// メンバ変数
	D3DXVECTOR3 m_move;		// 移動量
	const EType m_type;		// 種類定数
	int		m_nLife;		// 寿命
	float	m_fSubSize;		// 大きさの減算量
	float	m_fSubAlpha;	// 透明度の減算量
	CRenderState::EBlend	m_blend;	// αブレンド状況
};

#endif	// _EFFECT2D_H_
