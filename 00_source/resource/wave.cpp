//============================================================
//
//	波動処理 [wave.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "wave.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャファイル
	{
		nullptr,	// テクスチャなし
		"data\\TEXTURE\\wave000.png",	// 剣の波動
		"data\\TEXTURE\\wave001.png",	// 衝撃波の波動

	};

	const int PRIORITY = 4;	// 波動の優先順位
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CWave::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CWave] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CWave::CWave() : CScrollMeshRing(LABEL_WAVE, DIM_3D, PRIORITY)
{
	// メンバ変数をクリア
	memset(&m_grow, 0, sizeof(m_grow));	// 成長の情報
}

//============================================================
//	デストラクタ
//============================================================
CWave::~CWave()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CWave::Init(void)
{
	// メンバ変数を初期化
	m_grow = SGrow(0.0f, 0.0f, 0.0f);	// 成長の情報

	// スクロールメッシュリングの初期化
	if (FAILED(CScrollMeshRing::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// レンダーステートの情報を取得
	CRenderState *pRenderState = GetRenderState();

	// αテストの設定
	pRenderState->SetAlphaTest(true);			// αテストの有効 / 無効の設定
	pRenderState->SetAlphaFunc(D3DCMP_GREATER);	// αテストの設定

	// ポリゴンの両面を表示状態にする
	pRenderState->SetCulling(D3DCULL_NONE);

	// ライティングをOFFにする
	pRenderState->SetLighting(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CWave::Uninit(void)
{
	// スクロールメッシュリングの終了
	CScrollMeshRing::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CWave::Update(const float fDeltaTime)
{
	// 変数を宣言
	D3DXCOLOR colRing = GetColor();			// メッシュリングの色
	float fHoleRing  = GetHoleRadius();		// メッシュリングの穴の半径
	float fThickRing = GetThickness();		// メッシュリングの太さ

	// 波動を成長させる
	colRing.a  -= m_grow.fSubAlpha;			// 波動の透明度を減算 (透明にしていく)
	fHoleRing  += m_grow.fAddHoleRadius;	// 波動の穴の広がりを加算
	fThickRing += m_grow.fAddThickness;		// 波動を太さの広がりを加算

	if (fHoleRing < 0.0f)
	{ // 波動の穴が無くなった場合

		// 波動の穴の広がる量の余剰分を波動の太さに与える
		fThickRing += fHoleRing;

		// 波動の穴を補正
		fHoleRing = 0.0f;
	}

	if (colRing.a  <= 0.0f
	||  fThickRing <  0.0f)
	{ // 波動が透明、または細くなり切った場合

		// 波動オブジェクトの終了
		Uninit();

		// 処理を抜ける
		return;
	}

	// メッシュリングの色を反映
	SetColor(colRing);

	// メッシュリングの穴の半径を反映
	SetHoleRadius(fHoleRing);

	// メッシュリングの太さを反映
	SetThickness(fThickRing);

	// スクロールメッシュリングの更新
	CScrollMeshRing::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CWave::Draw(CShader *pShader)
{
	// スクロールメッシュリングの描画
	CScrollMeshRing::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CWave *CWave::Create
(
	const ETexture texture,		// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const SGrow& rGrow,			// 成長量
	const POSGRID2& rPart,		// 分割数
	const POSGRID2& rTexPart,	// テクスチャ分割数
	const float fHoleRadius,	// 穴の半径
	const float fThickness,		// 太さ
	const float fOuterPlusY		// 外周のY座標加算量
)
{
	// 波動の生成
	CWave *pWave = new CWave;
	if (pWave == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 波動の初期化
		if (FAILED(pWave->Init()))
		{ // 初期化に失敗した場合

			// 波動の破棄
			SAFE_DELETE(pWave);
			return nullptr;
		}

		// テクスチャを設定
		pWave->SetTexture(texture);

		// 位置を設定
		pWave->SetVec3Position(rPos);

		// 向きを設定
		pWave->SetVec3Rotation(rRot);

		// 色を設定
		pWave->SetColor(rCol);

		// 成長情報を設定
		pWave->SetGrow(rGrow);

		// 穴の半径を設定
		pWave->SetHoleRadius(fHoleRadius);

		// 太さを設定
		pWave->SetThickness(fThickness);

		// 外周のY座標加算量を設定
		pWave->SetOuterPlusY(fOuterPlusY);

		// 分割数を設定
		if (FAILED(pWave->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// オブジェクトメッシュリングの破棄
			SAFE_DELETE(pWave);
			return nullptr;
		}

		// テクスチャ分割数を設定
		pWave->SetTexPattern(rTexPart);

		// 確保したアドレスを返す
		return pWave;
	}
}

//============================================================
//	テクスチャの設定処理
//============================================================
void CWave::SetTexture(const ETexture texture)
{
	if (texture <= NONE_IDX || texture >= TEXTURE_MAX) { assert(false); return; }	// テクスチャインデックスが非正規

	// テクスチャを登録・割当
	BindTexture(GET_MANAGER->GetTexture()->Regist(TEXTURE_FILE[texture]));
}

//============================================================
//	成長情報の設定処理
//============================================================
void CWave::SetGrow(const SGrow & rGrow)
{
	// 引数の成長情報を設定
	m_grow = rGrow;
}

//============================================================
//	成長情報の取得処理
//============================================================
CWave::SGrow CWave::GetGrow(void) const
{
	// 成長情報を返す
	return m_grow;
}
