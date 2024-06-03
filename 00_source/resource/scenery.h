//============================================================
//
//	景色ヘッダー [scenery.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SCENERY_H_
#define _SCENERY_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshCylinder.h"

//************************************************************
//	クラス定義
//************************************************************
// 景色クラス
class CScenery : public CObjectMeshCylinder
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_MOUNTAIN_SMALL_00 = 0,	// 山テクスチャ
		TEXTURE_MOUNTAIN_SMALL_01,		// 山テクスチャ
		TEXTURE_MOUNTAIN_BIG,			// 山テクスチャ
		TEXTURE_MAX						// この列挙型の総数
	};

	// コンストラクタ
	CScenery();

	// デストラクタ
	~CScenery() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CScenery *Create	// 生成
	( // 引数
		const ETexture texture,		// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart,		// 分割数
		const POSGRID2& rTexPart,	// テクスチャ分割数
		const float fRadius,		// 半径
		const float fHeight			// 縦幅
	);
};

#endif	// _SCENERY_H_
