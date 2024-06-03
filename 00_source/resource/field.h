//============================================================
//
//	地面ヘッダー [field.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _FIELD_H_
#define _FIELD_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshField.h"

//************************************************************
//	クラス定義
//************************************************************
// 地面クラス
class CField : public CObjectMeshField
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_ASH = 0,	// 火山灰テクスチャ
		TEXTURE_MAX			// この列挙型の総数
	};

	// 地形列挙
	enum ETerrain
	{
		TERRAIN_120x120 = 0,	// 120x120分割数の地形
		TERRAIN_MAX				// この列挙型の総数
	};

	// コンストラクタ
	CField();

	// デストラクタ
	~CField() override;

	// 地形情報構造体
	struct STerrainInfo
	{
		D3DXVECTOR3 *pPosGap;	// 頂点座標のずれ量
		POSGRID2 part;			// 分割数
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static void LoadSetup(void);	// セットアップ
	static CField *Create	// 生成
	( // 引数
		const ETexture texture,		// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXVECTOR2& rSize,	// 大きさ
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart		// 分割数
	);

	// メンバ関数
	void SetTerrain(const ETerrain terrain);	// 地形設定

private:
	// 静的メンバ変数
	static STerrainInfo m_aTerrainInfo[TERRAIN_MAX];	// 地形情報
};

#endif	// _FIELD_H_
