//============================================================
//
//	空ヘッダー [sky.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SKY_H_
#define _SKY_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshDome.h"

//************************************************************
//	クラス定義
//************************************************************
// 空クラス
class CSky : public CObjectMeshDome
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_NORMAL = 0,	// 通常テクスチャ
		TEXTURE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CSky();

	// デストラクタ
	~CSky() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CSky *Create	// 生成
	( // 引数
		const ETexture texture,		// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart,		// 分割数
		const POSGRID2& rTexPart,	// テクスチャ分割数
		const float fRadius			// 半径
	);
};

#endif	// _SKY_H_
