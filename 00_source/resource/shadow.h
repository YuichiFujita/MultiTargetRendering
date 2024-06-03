//============================================================
//
//	影ヘッダー [shadow.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SHADOW_H_
#define _SHADOW_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object3D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace shadow
{
	const float	MIN_ALPHA = 0.25f;	// α値の最小値
	const float	MAX_ALPHA = 0.55f;	// α値の最大値
}

//************************************************************
//	クラス定義
//************************************************************
// 影クラス
class CShadow : public CObject3D
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_NORMAL = 0,	// 通常テクスチャ
		TEXTURE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CShadow(const float fMinAlpha, const float fMaxAlpha);

	// デストラクタ
	~CShadow() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CShadow *Create	// 生成
	( // 引数
		const ETexture texture,		// 種類
		const D3DXVECTOR3& rSize,	// 大きさ
		CObject *pObject,			// 親オブジェクト
		const float fMinAlpha = shadow::MIN_ALPHA,	// 透明度の最小値
		const float fMaxAlpha = shadow::MAX_ALPHA	// 透明度の最大値
	);

	// メンバ関数
	void SetMinAlpha(const float fMin) { m_fMinAlpha = fMin; }	// 透明度の最小値設定
	void SetMaxAlpha(const float fMax) { m_fMaxAlpha = fMax; }	// 透明度の最大値設定
	float GetMinAlpha(void) const { return m_fMinAlpha; }		// 透明度の最小値取得
	float GetMaxAlpha(void) const { return m_fMaxAlpha; }		// 透明度の最大値取得
	void SetSizingOrigin(const D3DXVECTOR3& rSize);				// 元の大きさ設定
	void SetDrawInfo(void);				// 描画情報設定
	D3DXVECTOR3 SetDrawPosition(void);	// 描画位置設定
	void DeleteObjectParent(void);		// 親オブジェクト削除

private:
	// メンバ関数
	void SetParentObject(CObject *pObject);	// 親オブジェクト設定

	// メンバ変数
	CObject *m_pParentObject;	// 親オブジェクト
	D3DXVECTOR3 m_sizeOrigin;	// 元の大きさ
	float m_fMinAlpha;			// 透明度の最小値
	float m_fMaxAlpha;			// 透明度の最大値
};

#endif	// _SHADOW_H_
