//============================================================
//
//	ゲージ2D処理 [gauge2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "gauge2D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int MAX_VERTEX	= 12;	// 頂点数
	const int PRIORITY		= 5;	// ゲージ2Dの優先順位
}

//************************************************************
//	子クラス [CGauge2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CGauge2D::CGauge2D(const int nFrame) : CObject(LABEL_UI, DIM_2D, PRIORITY),
	m_nFrame			(nFrame),		// 表示値の変動フレーム定数
	m_pVtxBuff			(nullptr),		// 頂点バッファへのポインタ
	m_pos				(VEC3_ZERO),	// 位置
	m_offsetFrame		(VEC3_ZERO),	// 枠オフセット
	m_sizeGauge			(VEC3_ZERO),	// ゲージ大きさ
	m_sizeFrame			(VEC3_ZERO),	// 枠大きさ
	m_colFront			(XCOL_WHITE),	// 表ゲージ色
	m_colBack			(XCOL_WHITE),	// 裏ゲージ色
	m_state				(STATE_NONE),	// 状態
	m_nNumGauge			(0),			// 表示値
	m_fChange			(0.0f),			// ゲージ変動量
	m_fAddRight			(0.0f),			// 横幅加算量
	m_nCounterState		(0),			// 状態管理カウンター
	m_nMaxNumGauge		(0),			// 表示値の最大値
	m_fCurrentNumGauge	(0.0f),			// 現在表示値
	m_bDrawFrame		(false)			// 枠表示状況
{
	// メンバ変数をクリア
	memset(&m_aTextureID[0], 0, sizeof(m_aTextureID));	// テクスチャインデックス
}

//============================================================
//	デストラクタ
//============================================================
CGauge2D::~CGauge2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CGauge2D::Init(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// メンバ変数を初期化
	m_pVtxBuff			= nullptr;		// 頂点バッファへのポインタ
	m_pos				= VEC3_ZERO;	// 位置
	m_offsetFrame		= VEC3_ZERO;	// 枠オフセット
	m_sizeGauge			= VEC3_ZERO;	// ゲージ大きさ
	m_sizeFrame			= VEC3_ZERO;	// 枠大きさ
	m_colFront			= XCOL_WHITE;	// 表ゲージ色
	m_colBack			= XCOL_WHITE;	// 裏ゲージ色
	m_state				= STATE_NONE;	// 状態
	m_nNumGauge			= 0;			// 表示値
	m_fChange			= 0.0f;			// ゲージ変動量
	m_fAddRight			= 0.0f;			// 横幅加算量
	m_nCounterState		= 0;			// 状態管理カウンター
	m_nMaxNumGauge		= 0;			// 表示値の最大値
	m_fCurrentNumGauge	= 0.0f;			// 現在表示値
	m_bDrawFrame		= false;		// 枠表示状況

	for (int nCntTexture = 0; nCntTexture < POLYGON_MAX; nCntTexture++)
	{ // 使用する四角形ポリゴン数分繰り返す

		// テクスチャインデックスをクリア
		m_aTextureID[nCntTexture] = NONE_IDX;
	}

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_2D) * MAX_VERTEX,	// 必要頂点数
		D3DUSAGE_WRITEONLY,				// 使用方法
		object::FVF_VERTEX_2D,			// 頂点フォーマット
		D3DPOOL_MANAGED,				// メモリの指定
		&m_pVtxBuff,					// 頂点バッファへのポインタ
		nullptr
	)))
	{ // 頂点バッファの生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 頂点情報の設定
	SetVtx();

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CGauge2D::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CGauge2D::Update(const float fDeltaTime)
{
	// ゲージの設定
	if (m_state == STATE_CHANGE)
	{ // 体力が変動中の場合

		// カウンターを減算
		m_nCounterState--;

		// 現在のゲージに変動量を加算
		m_fCurrentNumGauge += m_fChange;

		// 現在の表示値を補正
		useful::LimitNum(m_fCurrentNumGauge, 0.0f, (float)m_nMaxNumGauge);

		// ゲージの横幅加算量を設定
		m_fAddRight = (m_fCurrentNumGauge * ((m_sizeGauge.x * 2.0f) / (float)m_nMaxNumGauge)) - m_sizeGauge.x;

		// 頂点情報の設定
		SetVtx();

		if (m_nCounterState <= 0)
		{ // カウンターが 0以下になった場合

			// 通常状態にする
			m_state = STATE_NONE;
		}
	}
}

//============================================================
//	描画処理
//============================================================
void CGauge2D::Draw(CShader * /*pShader*/)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(object::FVF_VERTEX_2D);

	for (int nCntGauge = 0; nCntGauge < POLYGON_MAX; nCntGauge++)
	{ // 使用する四角形ポリゴン数分繰り返す

		if (nCntGauge == POLYGON_FRAME)
		{ // 描画する四角形ポリゴンが枠の場合

			if (m_bDrawFrame)
			{ // 枠を表示する場合

				// テクスチャの設定
				pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_aTextureID[nCntGauge]));

				// ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntGauge * 4, 2);
			}
		}
		else
		{ // 描画する四角形ポリゴンが枠以外の場合

			// テクスチャの設定
			pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_aTextureID[nCntGauge]));

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntGauge * 4, 2);
		}
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CGauge2D::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	生成処理
//============================================================
CGauge2D *CGauge2D::Create
(
	const int nMax,					// 最大表示値
	const int nFrame,				// 表示値変動フレーム
	const D3DXVECTOR3& rPos,		// 位置
	const D3DXVECTOR3& rSizeGauge,	// ゲージ大きさ
	const D3DXCOLOR& rColFront,		// 表ゲージ色
	const D3DXCOLOR& rColBack,		// 裏ゲージ色
	const bool bDrawFrame,			// 枠描画状況
	const char *pPassTex,			// フレームテクスチャパス
	const D3DXVECTOR3& rSizeFrame,	// 枠大きさ
	const D3DXVECTOR3& rOffsetFrame	// 枠オフセット
)
{
	// ゲージ2Dの生成
	CGauge2D *pGauge2D = new CGauge2D(nFrame);
	if (pGauge2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// ゲージ2Dの初期化
		if (FAILED(pGauge2D->Init()))
		{ // 初期化に失敗した場合

			// ゲージ2Dの破棄
			SAFE_DELETE(pGauge2D);
			return nullptr;
		}

		// テクスチャを登録・割当
		pGauge2D->BindTexture(POLYGON_FRAME, pPassTex);

		// ゲージ最大値を設定
		pGauge2D->SetMaxNum(nMax);

		// 位置を設定
		pGauge2D->SetVec3Position(rPos);

		// オフセットを設定
		pGauge2D->SetOffsetFrame(rOffsetFrame);

		// 大きさを設定
		pGauge2D->SetSizingGauge(rSizeGauge);	// ゲージ大きさ
		pGauge2D->SetSizingFrame(rSizeFrame);	// 枠大きさ

		// 色を設定
		pGauge2D->SetColorFront(rColFront);	// 表ゲージ色
		pGauge2D->SetColorBack(rColBack);	// 裏ゲージ色

		// 枠の表示状況を設定
		pGauge2D->SetEnableDrawFrame(bDrawFrame);

		// 確保したアドレスを返す
		return pGauge2D;
	}
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CGauge2D::BindTexture(const int nPolygonID, const int nTextureID)
{
	if (nPolygonID > NONE_IDX && nPolygonID < POLYGON_MAX)
	{ // 正規インデックスの場合

		if (nTextureID >= NONE_IDX)
		{ // テクスチャインデックスが使用可能な場合

			// テクスチャインデックスを代入
			m_aTextureID[nPolygonID] = nTextureID;
		}
		else { assert(false); }	// 範囲外
	}
}

//============================================================
//	テクスチャ割当処理 (パス)
//============================================================
void CGauge2D::BindTexture(const int nPolygonID, const char *pTexturePass)
{
	// ポインタを宣言
	CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ

	if (nPolygonID > NONE_IDX && nPolygonID < POLYGON_MAX)
	{ // 正規インデックスの場合

		if (pTexturePass != nullptr)
		{ // 割り当てるテクスチャパスがある場合
	
			// テクスチャインデックスを設定
			m_aTextureID[nPolygonID] = pTexture->Regist(pTexturePass);
		}
		else
		{ // 割り当てるテクスチャパスがない場合

			// テクスチャなしインデックスを設定
			m_aTextureID[nPolygonID] = NONE_IDX;
		}
	}
}

//============================================================
//	テクスチャインデックス取得処理
//============================================================
int CGauge2D::GetTextureIndex(const int nPolygonID) const
{
	// 非正規なポリゴンのインデックスの場合抜ける
	if (nPolygonID <= NONE_IDX || nPolygonID >= POLYGON_MAX) { assert(false); return NONE_IDX; }

	// テクスチャインデックスを返す
	return m_aTextureID[nPolygonID];
}

//============================================================
//	ゲージの加算処理
//============================================================
void CGauge2D::AddNum(const int nAdd)
{
	// 現在の表示値を設定
	m_fCurrentNumGauge = (float)m_nNumGauge;

	// 表示値の変動量を求める
	m_fChange = ((float)nAdd / (float)m_nFrame);

	// 情報を設定
	m_state = STATE_CHANGE;		// ゲージ変動状態
	m_nCounterState = m_nFrame;	// 状態管理カウンター

	// 表示値を引数分加算
	m_nNumGauge += nAdd;

	// 表示値の制限
	useful::LimitNum(m_nNumGauge, 0, m_nMaxNumGauge);
}

//============================================================
//	ゲージの設定処理
//============================================================
void CGauge2D::SetNum(const int nNum)
{
	// 引数の表示値を設定
	m_nNumGauge = nNum;
	useful::LimitNum(m_nNumGauge, 0, m_nMaxNumGauge);	// 表示値の制限

	// 現在の表示値を設定
	m_fCurrentNumGauge = (float)m_nNumGauge;
	useful::LimitNum(m_fCurrentNumGauge, 0.0f, (float)m_nMaxNumGauge);	// 現在の表示値の制限

	// 情報を設定
	m_state = STATE_NONE;	// ゲージ変動状態
	m_nCounterState = 0;	// 状態管理カウンター

	// ゲージの横幅加算量を設定
	m_fAddRight = (m_fCurrentNumGauge * ((m_sizeGauge.x * 2.0f) / (float)m_nMaxNumGauge)) - m_sizeGauge.x;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	ゲージ最大値の設定処理
//============================================================
void CGauge2D::SetMaxNum(const int nMax)
{
	// 引数の表示最大値を設定
	m_nMaxNumGauge = nMax;

	// ゲージの設定
	SetNum(m_nMaxNumGauge);
}

//============================================================
//	枠オフセットの設定処理
//============================================================
void CGauge2D::SetOffsetFrame(const D3DXVECTOR3& rOffset)
{
	// 引数の枠オフセットを代入
	m_offsetFrame = rOffset;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	ゲージ大きさの設定処理
//============================================================
void CGauge2D::SetSizingGauge(const D3DXVECTOR3& rSize)
{
	// 引数のゲージ大きさを代入
	m_sizeGauge = rSize;

	// ゲージの設定
	SetNum(m_nNumGauge);

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	背景大きさの設定処理
//============================================================
void CGauge2D::SetSizingFrame(const D3DXVECTOR3& rSize)
{
	// 引数の背景大きさを代入
	m_sizeFrame = rSize;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	表ゲージ色の設定処理
//============================================================
void CGauge2D::SetColorFront(const D3DXCOLOR& rCol)
{
	// 引数の表ゲージ色を代入
	m_colFront = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	裏ゲージ色の設定処理
//============================================================
void CGauge2D::SetColorBack(const D3DXCOLOR& rCol)
{
	// 引数の裏ゲージ色を代入
	m_colBack = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	枠表示状況設定処理
//============================================================
void CGauge2D::SetEnableDrawFrame(const bool bDraw)
{
	// 引数の枠の表示状況を設定
	m_bDrawFrame = bDraw;
}

//============================================================
//	頂点情報の設定処理
//============================================================
void CGauge2D::SetVtx(void)
{
	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntGauge = 0; nCntGauge < POLYGON_MAX; nCntGauge++)
		{ // 使用する四角形ポリゴン数分繰り返す

			switch (nCntGauge)
			{ // 四角形ポリゴンごとの処理
			case POLYGON_BACK:	// 背景

				// 頂点座標を設定
				pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_sizeGauge.x, m_pos.y - m_sizeGauge.y, 0.0f);
				pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_sizeGauge.x, m_pos.y - m_sizeGauge.y, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_sizeGauge.x, m_pos.y + m_sizeGauge.y, 0.0f);
				pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_sizeGauge.x, m_pos.y + m_sizeGauge.y, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = m_colBack;
				pVtx[1].col = m_colBack;
				pVtx[2].col = m_colBack;
				pVtx[3].col = m_colBack;

				break;

			case POLYGON_FRONT:	// ゲージ

				// 頂点座標を設定
				pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_sizeGauge.x, m_pos.y - m_sizeGauge.y, 0.0f);
				pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fAddRight,   m_pos.y - m_sizeGauge.y, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_sizeGauge.x, m_pos.y + m_sizeGauge.y, 0.0f);
				pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fAddRight,   m_pos.y + m_sizeGauge.y, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = m_colFront;
				pVtx[1].col = m_colFront;
				pVtx[2].col = m_colFront;
				pVtx[3].col = m_colFront;

				break;

			case POLYGON_FRAME:	// 枠

				// 頂点座標を設定
				pVtx[0].pos = D3DXVECTOR3(m_pos.x + m_offsetFrame.x - m_sizeFrame.x, m_pos.y + m_offsetFrame.y - m_sizeFrame.y, 0.0f);
				pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_offsetFrame.x + m_sizeFrame.x, m_pos.y + m_offsetFrame.y - m_sizeFrame.y, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(m_pos.x + m_offsetFrame.x - m_sizeFrame.x, m_pos.y + m_offsetFrame.y + m_sizeFrame.y, 0.0f);
				pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_offsetFrame.x + m_sizeFrame.x, m_pos.y + m_offsetFrame.y + m_sizeFrame.y, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = XCOL_WHITE;
				pVtx[1].col = XCOL_WHITE;
				pVtx[2].col = XCOL_WHITE;
				pVtx[3].col = XCOL_WHITE;

				break;

			default:	// 例外処理
				assert(false);
				break;
			}

			// rhw の設定
			pVtx[0].rhw = 1.0f;
			pVtx[1].rhw = 1.0f;
			pVtx[2].rhw = 1.0f;
			pVtx[3].rhw = 1.0f;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

			// 頂点データのポインタを 4つ分進める
			pVtx += 4;
		}

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}
