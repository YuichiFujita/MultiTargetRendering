//============================================================
//
//	単色描画シェーダーヘッダー [shaderMono.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SHADER_STENCIL_H_
#define _SHADER_STENCIL_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "shader.h"

//************************************************************
//	クラス定義
//************************************************************
// 単色描画シェーダークラス
class CMonoShader : public CShader
{
public:
	// コンストラクタ
	CMonoShader();

	// デストラクタ
	~CMonoShader() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了

	// メンバ関数
	void SetColor(const D3DXCOLOR& rCol);	// ピクセル描画色の設定

	// 静的メンバ関数
	static CMonoShader *Create(void);		// 生成
	static CMonoShader *GetInstance(void);	// 取得
	static void Release(void);					// 破棄

private:
	// 静的メンバ変数
	static CMonoShader *m_pShader;	// シェーダー情報

	// メンバ変数
	D3DXHANDLE m_pColDraw;	// ピクセル描画色
};

#endif	// _SHADER_STENCIL_H_
