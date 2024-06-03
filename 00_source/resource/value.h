//============================================================
//
//	数字ヘッダー [value.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _VALUE_H_
#define _VALUE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "anim2D.h"

//************************************************************
//	クラス定義
//************************************************************
// 数字クラス
class CValue : public CAnim2D
{
public:
	// 種類列挙
	enum EType
	{
		TYPE_NORMAL = 0,	// 通常テクスチャ
		TYPE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CValue();

	// デストラクタ
	~CValue() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CValue *Create	// 生成
	( // 引数
		const EType type = TYPE_NORMAL,			// 数字種類
		const D3DXVECTOR3& rPos = VEC3_ZERO,	// 位置
		const D3DXVECTOR3& rSize = VEC3_ONE,	// 大きさ
		const D3DXVECTOR3& rRot = VEC3_ZERO,	// 向き
		const D3DXCOLOR& rCol = XCOL_WHITE		// 色
	);

	// メイン関数
	void SetType(const EType type);	// 種類設定
	void SetNumber(const int nNum);	// 値設定

private:
	// メンバ変数
	int m_nNum;	// 表示数値
};

#endif	// _VALUE_H_
