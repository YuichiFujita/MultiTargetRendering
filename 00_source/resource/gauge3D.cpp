//============================================================
//
//	ゲージ3D処理 [gauge3D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "gauge3D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int MAX_VERTEX	= 12;	// 頂点数
	const int PRIORITY		= 6;	// ゲージ3Dの優先順位
}

//************************************************************
//	子クラス [CGauge3D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CGauge3D::CGauge3D(const int nFrame) : CObject(LABEL_UI, DIM_3D, PRIORITY),
	m_nFrame	(nFrame),		// 表示値の変動フレーム定数
	m_pVtxBuff	(nullptr),		// 頂点バッファへのポインタ
	m_pos		(VEC3_ZERO),	// 位置
	m_sizeGauge	(VEC3_ZERO),	// ゲージ大きさ
	m_sizeFrame	(VEC3_ZERO),	// 枠大きさ
	m_colFront	(XCOL_WHITE),	// 表ゲージ色
	m_colBack	(XCOL_WHITE),	// 裏ゲージ色
	m_state		(STATE_NONE),	// 状態
	m_nNumGauge	(0),			// 表示値
	m_fChange	(0.0f),			// ゲージ変動量
	m_fAddRight	(0.0f),			// 横幅加算量
	m_fPosUp	(0.0f),			// 表示Y位置の加算量
	m_nCounterState		(0),	// 状態管理カウンター
	m_nMaxNumGauge		(0),	// 表示値の最大値
	m_fCurrentNumGauge	(0.0f),	// 現在表示値
	m_bDrawFrame		(false)	// 枠表示状況
{
	// メンバ変数をクリア
	memset(&m_aTextureID[0], 0, sizeof(m_aTextureID));	// テクスチャインデックス
}

//============================================================
//	デストラクタ
//============================================================
CGauge3D::~CGauge3D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CGauge3D::Init(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// メンバ変数を初期化
	m_pVtxBuff	= nullptr;			// 頂点バッファへのポインタ
	m_pos		= VEC3_ZERO;		// 位置
	m_sizeGauge	= VEC3_ZERO;		// ゲージ大きさ
	m_sizeFrame	= VEC3_ZERO;		// 枠大きさ
	m_colFront	= XCOL_WHITE;		// 表ゲージ色
	m_colBack	= XCOL_WHITE;		// 裏ゲージ色
	m_state		= STATE_NONE;		// 状態
	m_nNumGauge	= 0;				// 表示値
	m_fChange	= 0.0f;				// ゲージ変動量
	m_fAddRight	= 0.0f;				// 横幅加算量
	m_fPosUp	= 0.0f;				// 表示Y位置の加算量
	m_nCounterState		= 0;		// 状態管理カウンター
	m_nMaxNumGauge		= 0;		// 表示値の最大値
	m_fCurrentNumGauge	= 0.0f;		// 現在表示値
	m_bDrawFrame		= false;	// 枠表示状況

	for (int nCntTexture = 0; nCntTexture < POLYGON_MAX; nCntTexture++)
	{ // 使用する四角形ポリゴン数分繰り返す

		// テクスチャインデックスをクリア
		m_aTextureID[nCntTexture] = NONE_IDX;
	}

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_3D) * MAX_VERTEX,	// 必要頂点数
		D3DUSAGE_WRITEONLY,				// 使用方法
		object::FVF_VERTEX_3D,			// 頂点フォーマット
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
void CGauge3D::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// ゲージ表示オブジェクトの削除
	DeleteGauge();

	// オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CGauge3D::Update(const float fDeltaTime)
{
	// 変数を宣言
	D3DXMATRIX mtxGauge;	// ゲージ表示オブジェクトのマトリックス

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

	// 位置の設定
	for (int nCntGauge = 0; nCntGauge < POLYGON_MAX; nCntGauge++)
	{ // 使用する四角形ポリゴン数分繰り返す

		if (m_pGauge != nullptr)
		{ // ゲージ表示対象が使用されていた場合

			// ワールドマトリックスを取得
			mtxGauge = m_pGauge->GetMtxWorld();

			// 位置を設定
			m_pos = D3DXVECTOR3(mtxGauge._41, mtxGauge._42 + m_fPosUp, mtxGauge._43);
		}
		else
		{ // ゲージ表示対象が使用されていなかった場合

			// ゲージ表示しているオブジェクトの情報を初期化
			m_pGauge = nullptr;

			// オブジェクトの終了
			Uninit();

			// 関数を抜ける
			return;
		}
	}
}

//============================================================
//	描画処理
//============================================================
void CGauge3D::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, false);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);	// Zテストの設定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);		// Zバッファ更新の有効 / 無効の設定

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&m_mtxWorld, nullptr, &mtxView);	// 逆行列を求める

	// マトリックスのワールド座標を原点にする
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(object::FVF_VERTEX_3D);

	if (pShader == nullptr)
	{ // シェーダーが使用されていない場合

		// 通常描画
		DrawNormal();
	}
	else
	{ // シェーダーが使用されている場合

		// シェーダー描画
		DrawShader(pShader);
	}

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, true);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);	// Zテストの設定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);		// Zバッファ更新の有効 / 無効の設定
}

//============================================================
//	位置の設定処理
//============================================================
void CGauge3D::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	生成処理
//============================================================
CGauge3D *CGauge3D::Create
(
	CObject *pObject,				// ゲージ表示オブジェクト
	const int nMax,					// 最大表示値
	const int nFrame,				// 表示値変動フレーム
	const float fPosUp,				// 表示Y位置の加算量
	const D3DXVECTOR3& rSizeGauge,	// ゲージ大きさ
	const D3DXCOLOR& rColFront,		// 表ゲージ色
	const D3DXCOLOR& rColBack,		// 裏ゲージ色
	const bool bDrawFrame,			// 枠描画状況
	const char *pPassTex,			// フレームテクスチャパス
	const D3DXVECTOR3& rSizeFrame	// 枠大きさ
)
{
	// ゲージ3Dの生成
	CGauge3D *pGauge3D = new CGauge3D(nFrame);
	if (pGauge3D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// ゲージ3Dの初期化
		if (FAILED(pGauge3D->Init()))
		{ // 初期化に失敗した場合

			// ゲージ3Dの破棄
			SAFE_DELETE(pGauge3D);
			return nullptr;
		}

		// テクスチャを登録・割当
		pGauge3D->BindTexture(POLYGON_FRAME, pPassTex);

		// ゲージ表示オブジェクトを設定
		pGauge3D->SetGaugeObject(pObject);

		// Y位置加算量を設定
		pGauge3D->SetPositionUp(fPosUp);

		// ゲージ最大値を設定
		pGauge3D->SetMaxNum(nMax);

		// 大きさを設定
		pGauge3D->SetSizingGauge(rSizeGauge);	// ゲージ大きさ
		pGauge3D->SetSizingFrame(rSizeFrame);	// 枠大きさ

		// 色を設定
		pGauge3D->SetColorFront(rColFront);	// 表ゲージ色
		pGauge3D->SetColorBack(rColBack);	// 裏ゲージ色

		// 枠の表示状況を設定
		pGauge3D->SetEnableDrawFrame(bDrawFrame);

		// 確保したアドレスを返す
		return pGauge3D;
	}
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CGauge3D::BindTexture(const int nPolygonID, const int nTextureID)
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
void CGauge3D::BindTexture(const int nPolygonID, const char *pTexturePass)
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
int CGauge3D::GetTextureIndex(const int nPolygonID) const
{
	// 非正規なポリゴンのインデックスの場合抜ける
	if (nPolygonID <= NONE_IDX || nPolygonID >= POLYGON_MAX) { assert(false); return NONE_IDX; }

	// テクスチャインデックスを返す
	return m_aTextureID[nPolygonID];
}

//============================================================
//	ゲージの加算処理
//============================================================
void CGauge3D::AddNum(const int nAdd)
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
void CGauge3D::SetNum(const int nNum)
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
void CGauge3D::SetMaxNum(const int nMax)
{
	// 引数の表示最大値を設定
	m_nMaxNumGauge = nMax;

	// ゲージの設定
	SetNum(m_nMaxNumGauge);
}

//============================================================
//	ゲージ表示オブジェクトの設定処理
//============================================================
void CGauge3D::SetGaugeObject(CObject *pObject)
{
	// 引数のオブジェクトを設定
	m_pGauge = pObject;
}

//============================================================
//	ゲージ表示オブジェクトの削除処理
//============================================================
void CGauge3D::DeleteGauge(void)
{
	// ゲージ3D表示オブジェクトをnullptrにする
	m_pGauge = nullptr;
}

//============================================================
//	ゲージ大きさの設定処理
//============================================================
void CGauge3D::SetSizingGauge(const D3DXVECTOR3& rSize)
{
	// 引数のゲージ大きさを代入
	m_sizeGauge = rSize;

	// ゲージの設定
	SetNum(m_nNumGauge);

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	枠大きさの設定処理
//============================================================
void CGauge3D::SetSizingFrame(const D3DXVECTOR3& rSize)
{
	// 引数の背景大きさを代入
	m_sizeFrame = rSize;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	表ゲージ色の設定処理
//============================================================
void CGauge3D::SetColorFront(const D3DXCOLOR& rCol)
{
	// 引数の表ゲージ色を代入
	m_colFront = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	裏ゲージ色の設定処理
//============================================================
void CGauge3D::SetColorBack(const D3DXCOLOR& rCol)
{
	// 引数の裏ゲージ色を代入
	m_colBack = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	Y位置加算量の設定処理
//============================================================
void CGauge3D::SetPositionUp(const float fUp)
{
	// 引数のY位置加算量を設定
	m_fPosUp = fUp;
}

//============================================================
//	枠表示状況設定処理
//============================================================
void CGauge3D::SetEnableDrawFrame(const bool bDraw)
{
	// 引数の枠の表示状況を設定
	m_bDrawFrame = bDraw;
}

//============================================================
//	頂点情報の設定処理
//============================================================
void CGauge3D::SetVtx(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

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
				pVtx[0].pos = D3DXVECTOR3(-m_sizeGauge.x * 0.5f,  m_sizeGauge.y * 0.5f, 0.0f);
				pVtx[1].pos = D3DXVECTOR3( m_sizeGauge.x * 0.5f,  m_sizeGauge.y * 0.5f, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(-m_sizeGauge.x * 0.5f, -m_sizeGauge.y * 0.5f, 0.0f);
				pVtx[3].pos = D3DXVECTOR3( m_sizeGauge.x * 0.5f, -m_sizeGauge.y * 0.5f, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = m_colBack;
				pVtx[1].col = m_colBack;
				pVtx[2].col = m_colBack;
				pVtx[3].col = m_colBack;

				break;

			case POLYGON_FRONT:	// ゲージ

				// 頂点座標を設定
				pVtx[0].pos = D3DXVECTOR3(-m_sizeGauge.x * 0.5f,  m_sizeGauge.y * 0.5f, 0.0f);
				pVtx[1].pos = D3DXVECTOR3( m_fAddRight   * 0.5f,  m_sizeGauge.y * 0.5f, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(-m_sizeGauge.x * 0.5f, -m_sizeGauge.y * 0.5f, 0.0f);
				pVtx[3].pos = D3DXVECTOR3( m_fAddRight   * 0.5f, -m_sizeGauge.y * 0.5f, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = m_colFront;
				pVtx[1].col = m_colFront;
				pVtx[2].col = m_colFront;
				pVtx[3].col = m_colFront;

				break;

			case POLYGON_FRAME:	// 枠

				// 頂点座標を設定
				pVtx[0].pos = D3DXVECTOR3(-m_sizeFrame.x * 0.5f,  m_sizeFrame.y * 0.5f, 0.0f);
				pVtx[1].pos = D3DXVECTOR3( m_sizeFrame.x * 0.5f,  m_sizeFrame.y * 0.5f, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(-m_sizeFrame.x * 0.5f, -m_sizeFrame.y * 0.5f, 0.0f);
				pVtx[3].pos = D3DXVECTOR3( m_sizeFrame.x * 0.5f, -m_sizeFrame.y * 0.5f, 0.0f);

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

			// 法線ベクトルの設定
			pVtx[0].nor = VEC3_ZERO;
			pVtx[1].nor = VEC3_ZERO;
			pVtx[2].nor = VEC3_ZERO;
			pVtx[3].nor = VEC3_ZERO;

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

//============================================================
//	通常描画処理
//============================================================
void CGauge3D::DrawNormal(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

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
//	シェーダー描画処理
//============================================================
void CGauge3D::DrawShader(CShader *pShader)
{
	// 変数配列を宣言
	D3DXCOLOR aCol[POLYGON_MAX] =	// 色情報
	{
		m_colBack,	// 背景
		m_colFront,	// ゲージ
		XCOL_WHITE	// 枠
	};

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 描画開始
	pShader->Begin();
	pShader->BeginPass(0);

	// マトリックス情報を設定
	pShader->SetMatrix(&m_mtxWorld);

	// ライト方向を設定
	pShader->SetLightDirect(&m_mtxWorld, 0);

	for (int nCntGauge = 0; nCntGauge < POLYGON_MAX; nCntGauge++)
	{ // 使用する四角形ポリゴン数分繰り返す

		// 拡散光を設定
		pShader->SetOnlyDiffuse(aCol[nCntGauge]);

		// テクスチャを設定
		pShader->SetTexture(m_aTextureID[nCntGauge]);

		// 状態変更の伝達
		pShader->CommitChanges();

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

	// 描画終了
	pShader->EndPass();
	pShader->End();
}
