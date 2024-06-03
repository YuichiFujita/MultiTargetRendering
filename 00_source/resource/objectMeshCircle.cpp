//============================================================
//
//	オブジェクトメッシュサークル処理 [objectMeshCircle.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshCircle.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const POSGRID2 MIN_PART = POSGRID2(1, 2);	// 分割数の最小値
}

//************************************************************
//	子クラス [CObjectMeshCircle] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectMeshCircle::CObjectMeshCircle(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_pVtxBuff		(nullptr),		// 頂点バッファ
	m_pIdxBuff		(nullptr),		// インデックスバッファ
	m_pRenderState	(nullptr),		// レンダーステートの情報
	m_part			(GRID2_ZERO),	// 分割数
	m_nNumVtx		(0),			// 必要頂点数
	m_nNumIdx		(0),			// 必要インデックス数
	m_nTextureID	(0)				// テクスチャインデックス
{
	// メンバ変数をクリア
	memset(&m_meshCircle, 0, sizeof(m_meshCircle));	// メッシュサークルの情報
}

//============================================================
//	デストラクタ
//============================================================
CObjectMeshCircle::~CObjectMeshCircle()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectMeshCircle::Init(void)
{
	// メンバ変数を初期化
	m_pVtxBuff		= nullptr;	// 頂点バッファ
	m_pIdxBuff		= nullptr;	// インデックスバッファ
	m_pRenderState	= nullptr;	// レンダーステートの情報
	m_part			= MIN_PART;	// 分割数
	m_nNumVtx		= 0;		// 必要頂点数
	m_nNumIdx		= 0;		// 必要インデックス数
	m_nTextureID	= NONE_IDX;	// テクスチャインデックス

	m_meshCircle.pos		= VEC3_ZERO;	// 位置
	m_meshCircle.rot		= VEC3_ZERO;	// 向き
	m_meshCircle.col		= XCOL_WHITE;	// 色
	m_meshCircle.fRadius	= 0.0f;			// 半径

	// 分割数を設定
	if (FAILED(SetPattern(MIN_PART)))
	{ // 分割数の設定に失敗した場合

		// 失敗を返す
		return E_FAIL;
	}

	// レンダーステートの生成
	m_pRenderState = CRenderState::Create();
	if (m_pRenderState == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CObjectMeshCircle::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// インデックスバッファの破棄
	SAFE_RELEASE(m_pIdxBuff);

	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// オブジェクトメッシュサークルを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectMeshCircle::Update(const float fDeltaTime)
{

}

//============================================================
//	描画処理
//============================================================
void CObjectMeshCircle::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// レンダーステートを設定
	m_pRenderState->Set();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_meshCircle.mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_meshCircle.rot.y, m_meshCircle.rot.x, m_meshCircle.rot.z);
	D3DXMatrixMultiply(&m_meshCircle.mtxWorld, &m_meshCircle.mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_meshCircle.pos.x, m_meshCircle.pos.y, m_meshCircle.pos.z);
	D3DXMatrixMultiply(&m_meshCircle.mtxWorld, &m_meshCircle.mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_meshCircle.mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

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

	// レンダーステートを再設定
	m_pRenderState->Reset();
}

//============================================================
//	位置の設定処理
//============================================================
void CObjectMeshCircle::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_meshCircle.pos = rPos;
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectMeshCircle::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_meshCircle.rot = rRot;

	// 向きの正規化
	useful::NormalizeRot(m_meshCircle.rot);
}

//============================================================
//	生成処理
//============================================================
CObjectMeshCircle *CObjectMeshCircle::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart,		// 分割数
	const float fRadius			// 半径
)
{
	// オブジェクトメッシュサークルの生成
	CObjectMeshCircle *pMeshCircle = new CObjectMeshCircle;
	if (pMeshCircle == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクトメッシュサークルの初期化
		if (FAILED(pMeshCircle->Init()))
		{ // 初期化に失敗した場合

			// オブジェクトメッシュサークルの破棄
			SAFE_DELETE(pMeshCircle);
			return nullptr;
		}

		// 位置を設定
		pMeshCircle->SetVec3Position(rPos);

		// 向きを設定
		pMeshCircle->SetVec3Rotation(rRot);

		// 色を設定
		pMeshCircle->SetColor(rCol);

		// 半径を設定
		pMeshCircle->SetRadius(fRadius);

		// 分割数を設定
		if (FAILED(pMeshCircle->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// オブジェクトメッシュサークルの破棄
			SAFE_DELETE(pMeshCircle);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pMeshCircle;
	}
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *CObjectMeshCircle::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObjectMeshCircle::BindTexture(const int nTextureID)
{
	if (nTextureID >= NONE_IDX)
	{ // テクスチャインデックスが使用可能な場合

		// テクスチャインデックスを代入
		m_nTextureID = nTextureID;
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	テクスチャ割当処理 (パス)
//============================================================
void CObjectMeshCircle::BindTexture(const char *pTexturePass)
{
	// ポインタを宣言
	CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ

	if (pTexturePass != nullptr)
	{ // 割り当てるテクスチャパスがある場合

		// テクスチャインデックスを設定
		m_nTextureID = pTexture->Regist(pTexturePass);
	}
	else
	{ // 割り当てるテクスチャパスがない場合

		// テクスチャなしインデックスを設定
		m_nTextureID = NONE_IDX;
	}
}

//============================================================
//	色の設定処理
//============================================================
void CObjectMeshCircle::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を設定
	m_meshCircle.col = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	半径の設定処理
//============================================================
void CObjectMeshCircle::SetRadius(const float fRadius)
{
	// 引数の半径を設定
	m_meshCircle.fRadius = fRadius;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	分割数の設定処理
//============================================================
HRESULT CObjectMeshCircle::SetPattern(const POSGRID2& rPart)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 分割数の設定不可
	assert(rPart.x >= MIN_PART.x);
	assert(rPart.y >= MIN_PART.y);

	// 引数の分割数を設定
	m_part = rPart;

	// 必要頂点・インデックス数を求める
	m_nNumVtx = ((m_part.x + 1) * (m_part.y + 1)) - m_part.x; // 必要頂点数
	m_nNumIdx = ((m_part.x + 1) * (((m_part.y + 1) * 2) - 2) + (m_part.y * 2) - 2) - (m_part.x + 2); // 必要インデックス数

	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
	assert(m_pVtxBuff == nullptr);

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_3D) * m_nNumVtx,	// 必要頂点数
		D3DUSAGE_WRITEONLY,		// 使用方法
		object::FVF_VERTEX_3D,	// 頂点フォーマット
		D3DPOOL_MANAGED,		// メモリの指定
		&m_pVtxBuff,			// 頂点バッファへのポインタ
		nullptr
	)))
	{ // 頂点バッファの生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// インデックスバッファの破棄
	SAFE_RELEASE(m_pIdxBuff);
	assert(m_pIdxBuff == nullptr);

	// インデックスバッファの生成
	if (FAILED(pDevice->CreateIndexBuffer
	( // 引数
		sizeof(WORD) * m_nNumIdx,	// 必要インデックス数
		D3DUSAGE_WRITEONLY,	// 使用方法
		D3DFMT_INDEX16,		// インデックスバッファのフォーマット
		D3DPOOL_MANAGED,	// メモリの指定
		&m_pIdxBuff,		// インデックスバッファへのポインタ
		nullptr
	)))
	{ // インデックスバッファの生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 頂点・インデックス情報の設定
	SetVtx();
	SetIdx();

	// 成功を返す
	return S_OK;
}

//============================================================
//	頂点情報の設定処理
//============================================================
void CObjectMeshCircle::SetVtx(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntHeight = 0; nCntHeight < m_part.y + 1; nCntHeight++)
		{ // 縦の分割数 +1回繰り返す

			// 繰り返し数を求める
			int nLoop = (m_part.x * (1 - (nCntHeight / m_part.y))) + 1;

			for (int nCntWidth = 0; nCntWidth < nLoop; nCntWidth++)
			{ // 横の分割数 +1回繰り返す (天辺の頂点時は繰り返しを 1回にする)

				// 頂点座標の方向を設定
				float fRotWidth  = D3DXToRadian(nCntWidth * (360 / (float)m_part.x));	// 頂点位置の設定方向
				float fDisHeight = (((m_meshCircle.fRadius / (float)m_part.y) * -(nCntHeight - m_part.y)));	// 頂点位置の設定距離
				D3DXVECTOR3 vecPos = D3DXVECTOR3
				( // 引数
					sinf(fRotWidth) * fDisHeight,	// x
					0.0f,							// y
					cosf(fRotWidth) * fDisHeight 	// z
				);

				// 頂点座標の設定
				pVtx[0].pos = vecPos;

				// 法線ベクトルの設定
				pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = m_meshCircle.col;

				// テクスチャ座標の設定
				pVtx[0].tex = D3DXVECTOR2
				( // 引数
					0.5f + sinf(fRotWidth) * (0.5f / m_meshCircle.fRadius) * fDisHeight,	// u
					0.5f + cosf(fRotWidth) * (0.5f / m_meshCircle.fRadius) * fDisHeight		// v
				);

				// 頂点データのポインタを 1つ分進める
				pVtx += 1;
			}
		}

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}

//============================================================
//	インデックス情報の設定処理
//============================================================
void CObjectMeshCircle::SetIdx(void)
{
	// ポインタを宣言
	WORD *pIdx;	// インデックス情報へのポインタ

	if (m_pIdxBuff != nullptr)
	{ // 使用中の場合

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntHeight = 0, nCntWidth = 0; nCntHeight < m_part.y - 1; nCntHeight++)
		{ // 縦の分割数 +1回繰り返す

			for (nCntWidth = 0; nCntWidth < m_part.x + 1; nCntWidth++)
			{ // 横の分割数 +1回繰り返す

				pIdx[0] = (WORD)((m_part.x + 1) * (nCntHeight + 1) + nCntWidth);
				pIdx[1] = (WORD)((m_part.x + 1) * nCntHeight + nCntWidth);

				// インデックスデータのポインタを 2つ分進める
				pIdx += 2;
			}

			if (nCntHeight != m_part.y - 2)
			{ // 一番手前の分割場所ではない場合

				pIdx[0] = (WORD)((m_part.x + 1) * nCntHeight + nCntWidth - 1);
				pIdx[1] = (WORD)((m_part.x + 1) * (nCntHeight + 2));

				// インデックスデータのポインタを 2つ分進める
				pIdx += 2;
			}
		}

		// 半球の天辺の頂点を指定
		pIdx[0] = (WORD)(m_nNumVtx - 1);

		// インデックスデータのポインタを 1つ分進める
		pIdx += 1;

		for (int nCntWidth = 0; nCntWidth < m_part.x + 1; nCntWidth++)
		{ // 横の分割数 +1回繰り返す

			pIdx[0] = (WORD)(((m_part.x + 1) * (m_part.y - 1)) + nCntWidth);

			// インデックスデータのポインタを 1つ分進める
			pIdx += 1;
		}

		// インデックスバッファをアンロックする
		m_pIdxBuff->Unlock();
	}
}

//============================================================
//	通常描画処理
//============================================================
void CObjectMeshCircle::DrawNormal(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// 外周ポリゴンの描画
	pDevice->DrawIndexedPrimitive
	( // 引数
		D3DPT_TRIANGLESTRIP,	// プリミティブの種類
		0,
		0,
		m_nNumVtx - 1,						// 使用する頂点数
		0,									// インデックスバッファの開始地点
		(m_nNumIdx - (m_part.x + 1)) - 3	// プリミティブ (ポリゴン) 数
	);

	// 内円ポリゴンの描画
	pDevice->DrawIndexedPrimitive
	( // 引数
		D3DPT_TRIANGLEFAN,	// プリミティブの種類
		0,
		0,
		m_part.x + 1,				// 使用する頂点数
		m_nNumIdx - (m_part.x + 2),	// インデックスバッファの開始地点
		m_part.x					// プリミティブ (ポリゴン) 数
	);
}

//============================================================
//	シェーダー描画処理
//============================================================
void CObjectMeshCircle::DrawShader(CShader *pShader)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 描画開始
	pShader->Begin();
	pShader->BeginPass(0);

	// マトリックス情報を設定
	pShader->SetMatrix(&m_meshCircle.mtxWorld);

	// ライト方向を設定
	pShader->SetLightDirect(&m_meshCircle.mtxWorld, 0);

	// 拡散光を設定
	pShader->SetOnlyDiffuse(m_meshCircle.col);

	// テクスチャを設定
	pShader->SetTexture(m_nTextureID);

	// 状態変更の伝達
	pShader->CommitChanges();

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// 外周ポリゴンの描画
	pDevice->DrawIndexedPrimitive
	( // 引数
		D3DPT_TRIANGLESTRIP,	// プリミティブの種類
		0,
		0,
		m_nNumVtx - 1,						// 使用する頂点数
		0,									// インデックスバッファの開始地点
		(m_nNumIdx - (m_part.x + 1)) - 3	// プリミティブ (ポリゴン) 数
	);

	// 内円ポリゴンの描画
	pDevice->DrawIndexedPrimitive
	( // 引数
		D3DPT_TRIANGLEFAN,	// プリミティブの種類
		0,
		0,
		m_part.x + 1,				// 使用する頂点数
		m_nNumIdx - (m_part.x + 2),	// インデックスバッファの開始地点
		m_part.x					// プリミティブ (ポリゴン) 数
	);

	// 描画終了
	pShader->EndPass();
	pShader->End();
}
