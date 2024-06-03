//============================================================
//
//	液体ヘッダー [liquid.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _LIQUID_H_
#define _LIQUID_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "scrollMeshField.h"

//************************************************************
//	クラス定義
//************************************************************
// 液体クラス
class CLiquid : public CObject
{
public:
	// 種類列挙
	enum EType
	{
		TYPE_LAVA = 0,	// マグマ
		TYPE_MAX		// この列挙型の総数
	};

	// 上下列挙
	enum ELiquid
	{
		LIQUID_LOW = 0,	// 下
		LIQUID_HIGH,	// 上
		LIQUID_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CLiquid();

	// デストラクタ
	~CLiquid() override;

	// テクスチャ移動量構造体
	struct STexMove
	{
	public:
		// コンストラクタ
		STexMove() {}
		STexMove(const D3DXVECTOR2 TexMoveLow, const D3DXVECTOR2 TexMoveHigh)
		{ texMoveLow = TexMoveLow; texMoveHigh = TexMoveHigh; }

		// デストラクタ
		~STexMove() {}

		// メンバ変数
		D3DXVECTOR2 texMoveLow;		// 下液体のテクスチャ移動量
		D3DXVECTOR2 texMoveHigh;	// 上液体のテクスチャ移動量
	};

	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;			// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	void SetVec3Rotation(const D3DXVECTOR3& rRot) override;	// 向き設定
	void SetVec2Sizing(const D3DXVECTOR2& rSize) override;	// 大きさ設定
	D3DXVECTOR3 GetVec3Position(void) const override	{ return m_apLiquid[LIQUID_LOW]->GetVec3Position(); }	// 位置取得
	D3DXVECTOR3 GetVec3Rotation(void) const override	{ return m_apLiquid[LIQUID_LOW]->GetVec3Rotation(); }	// 向き取得
	D3DXVECTOR2 GetVec2Sizing(void) const override		{ return m_apLiquid[LIQUID_LOW]->GetVec2Sizing(); }		// 大きさ取得
	D3DXMATRIX *GetPtrMtxWorld(void) override			{ return m_apLiquid[LIQUID_LOW]->GetPtrMtxWorld(); }	// マトリックスポインタ取得
	D3DXMATRIX GetMtxWorld(void) const override			{ return m_apLiquid[LIQUID_LOW]->GetMtxWorld(); }		// マトリックス取得

	// 静的メンバ関数
	static CLiquid *Create	// 生成
	( // 引数
		const EType type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXVECTOR2& rSize,	// 大きさ
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart,		// 分割数
		const STexMove& rTexMove,	// テクスチャ移動量
		const float fMaxUp,			// 波の最高上昇量
		const float fAddSinRot,		// 波打ち向き加算量
		const float fAddVtxRot		// 隣波の向き加算量
	);

	// メンバ関数
	void SetColor(const D3DXCOLOR& rCol);		// 色設定
	HRESULT SetPattern(const POSGRID2& rPart);	// 分割数設定
	void SetType(const EType type);				// 種類設定
	void SetTexMove(const STexMove texMove);	// テクスチャ移動量設定
	void SetMaxUp(const float fMaxUp);			// 波の最高上昇量設定
	void SetAddSinRot(const float fAddSinRot);	// 波打ち向き加算量設定
	void SetAddVtxRot(const float fAddVtxRot);	// 隣波の向き加算量設定
	D3DXCOLOR GetColor(void) const	{ return m_apLiquid[LIQUID_LOW]->GetColor(); }		// 色取得
	POSGRID2 GetPattern(void) const	{ return m_apLiquid[LIQUID_LOW]->GetPattern(); }	// 分割数取得
	EType GetType(void) const		{ return m_type; }			// 種類取得
	float GetMaxUp(void) const		{ return m_fMaxUp; }		// 波の最高上昇量取得
	float GetAddSinRot(void) const	{ return m_fAddSinRot; }	// 波打ち向き加算量取得
	float GetAddVtxRot(void) const	{ return m_fAddVtxRot; }	// 隣波の向き加算量取得

	STexMove GetTexMove(void) const;	// テクスチャ移動量取得
	CScrollMeshField *GetMeshField(const int nID) const;	// メッシュフィールド取得

private:
	// オーバーライド関数
	void Release(void) override { CObject::Release(); }	// 破棄

	// メンバ変数
	CScrollMeshField *m_apLiquid[LIQUID_MAX];	// 液体の情報
	EType m_type;		// 種類
	float m_fMaxUp;		// 波の最高上昇量
	float m_fSinRot;	// 波打ち向き
	float m_fAddSinRot;	// 波打ち向き加算量
	float m_fAddVtxRot;	// 隣波の向き加算量
};

#endif	// _LIQUID_H_
