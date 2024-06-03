//============================================================
//
//	マルチモデルヘッダー [multiModel.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MULTIMODEL_H_
#define _MULTIMODEL_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "objectModel.h"

//************************************************************
//	クラス定義
//************************************************************
// マルチモデルクラス
class CMultiModel : public CObjectModel
{
public:
	// コンストラクタ
	explicit CMultiModel(const CObject::ELabel label = LABEL_NONE, const CObject::EDim dimension = DIM_3D, const int nPriority = object::DEFAULT_PRIO);

	// デストラクタ
	~CMultiModel() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CMultiModel *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,				// 位置
		const D3DXVECTOR3& rRot,				// 向き
		const D3DXVECTOR3& rScale = VEC3_ONE	// 大きさ
	);

	// メンバ関数
	void SetParentObject(CObject *pObject);		// 親オブジェクト設定
	void SetParentModel(CMultiModel *pModel);	// 親オブジェクト設定
	void DeleteParentObject(void);				// 親オブジェクト削除

private:
	// メンバ関数
	void DrawNormal(void);	// 通常描画
	void DrawShader(CShader *pShader);	// シェーダー描画

	// メンバ変数
	CObject *m_pParent;	// 親オブジェクトへのポインタ
};

#endif	// _MULTIMODEL_H_
