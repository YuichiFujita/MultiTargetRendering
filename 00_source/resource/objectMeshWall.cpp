//============================================================
//
//	オブジェクトメッシュウォール処理 [objectMeshWall.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshWall.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	子クラス [CObjectMeshWall] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectMeshWall::CObjectMeshWall(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_pVtxBuff		(nullptr),		// 頂点バッファ
	m_pIdxBuff		(nullptr),		// インデックスバッファ
	m_pRenderState	(nullptr),		// レンダーステートの情報
	m_part			(GRID2_ZERO),	// 分割数
	m_nNumVtx		(0),			// 必要頂点数
	m_nNumIdx		(0),			// 必要インデックス数
	m_nTextureID	(0)				// テクスチャインデックス
{
	// メンバ変数をクリア
	memset(&m_meshWall, 0, sizeof(m_meshWall));	// メッシュウォールの情報
}

//============================================================
//	デストラクタ
//============================================================
CObjectMeshWall::~CObjectMeshWall()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectMeshWall::Init(void)
{
	// メンバ変数を初期化
	m_pVtxBuff		= nullptr;		// 頂点バッファ
	m_pIdxBuff		= nullptr;		// インデックスバッファ
	m_pRenderState	= nullptr;		// レンダーステートの情報
	m_part			= GRID2_ZERO;	// 分割数
	m_nNumVtx		= 0;			// 必要頂点数
	m_nNumIdx		= 0;			// 必要インデックス数
	m_nTextureID	= NONE_IDX;		// テクスチャインデックス

	m_meshWall.pos		= VEC3_ZERO;	// 位置
	m_meshWall.rot		= VEC3_ZERO;	// 向き
	m_meshWall.size		= VEC2_ZERO;	// 大きさ
	m_meshWall.col		= XCOL_WHITE;	// 色

	// 分割数を設定
	if (FAILED(SetPattern(GRID2_ONE)))
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
void CObjectMeshWall::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// インデックスバッファの破棄
	SAFE_RELEASE(m_pIdxBuff);

	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// オブジェクトメッシュウォールを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectMeshWall::Update(const float fDeltaTime)
{

}

//============================================================
//	描画処理
//============================================================
void CObjectMeshWall::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// レンダーステートを設定
	m_pRenderState->Set();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_meshWall.mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_meshWall.rot.y, m_meshWall.rot.x, m_meshWall.rot.z);
	D3DXMatrixMultiply(&m_meshWall.mtxWorld, &m_meshWall.mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_meshWall.pos.x, m_meshWall.pos.y, m_meshWall.pos.z);
	D3DXMatrixMultiply(&m_meshWall.mtxWorld, &m_meshWall.mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_meshWall.mtxWorld);

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
void CObjectMeshWall::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_meshWall.pos = rPos;
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectMeshWall::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_meshWall.rot = rRot;

	// 向きの正規化
	useful::NormalizeRot(m_meshWall.rot);
}

//============================================================
//	大きさの設定処理
//============================================================
void CObjectMeshWall::SetVec2Sizing(const D3DXVECTOR2& rSize)
{
	// 引数の大きさを設定
	m_meshWall.size = rSize;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	生成処理
//============================================================
CObjectMeshWall *CObjectMeshWall::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR2& rSize,	// 大きさ
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart		// 分割数
)
{
	// オブジェクトメッシュウォールの生成
	CObjectMeshWall *pMeshWall = new CObjectMeshWall;
	if (pMeshWall == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクトメッシュウォールの初期化
		if (FAILED(pMeshWall->Init()))
		{ // 初期化に失敗した場合

			// オブジェクトメッシュウォールの破棄
			SAFE_DELETE(pMeshWall);
			return nullptr;
		}

		// 位置を設定
		pMeshWall->SetVec3Position(rPos);

		// 向きを設定
		pMeshWall->SetVec3Rotation(rRot);

		// 大きさを設定
		pMeshWall->SetVec2Sizing(rSize);

		// 色を設定
		pMeshWall->SetColor(rCol);

		// 分割数を設定
		if (FAILED(pMeshWall->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// オブジェクトメッシュウォールの破棄
			SAFE_DELETE(pMeshWall);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pMeshWall;
	}
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *CObjectMeshWall::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObjectMeshWall::BindTexture(const int nTextureID)
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
void CObjectMeshWall::BindTexture(const char *pTexturePass)
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
void CObjectMeshWall::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を設定
	m_meshWall.col = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	分割数の設定処理
//============================================================
HRESULT CObjectMeshWall::SetPattern(const POSGRID2& rPart)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 引数の分割数を設定
	m_part = rPart;

	// 必要頂点・インデックス数を求める
	m_nNumVtx = (m_part.x + 1) * (m_part.y + 1); // 必要頂点数
	m_nNumIdx = (m_part.x + 1) * (((m_part.y + 1) * 2) - 2) + (m_part.y * 2) - 2; // 必要インデックス数

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
void CObjectMeshWall::SetVtx(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntHeight = 0; nCntHeight < m_part.y + 1; nCntHeight++)
		{ // 縦の分割数 +1回繰り返す

			for (int nCntWidth = 0; nCntWidth < m_part.x + 1; nCntWidth++)
			{ // 横の分割数 +1回繰り返す

				// 頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3
				( // 引数
					nCntWidth * (m_meshWall.size.x / (float)m_part.x) - (m_meshWall.size.x * 0.5f),	// x
					-(nCntHeight * (m_meshWall.size.y / (float)m_part.y)) + m_meshWall.size.y,		// y
					0.0f																			// z
				);

				// 法線ベクトルの設定
				pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

				// 頂点カラーの設定
				pVtx[0].col = m_meshWall.col;

				// テクスチャ座標の設定
				pVtx[0].tex = D3DXVECTOR2(1.0f * nCntWidth, 1.0f * nCntHeight);

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
void CObjectMeshWall::SetIdx(void)
{
	// ポインタを宣言
	WORD *pIdx;	// インデックス情報へのポインタ

	if (m_pIdxBuff != nullptr)
	{ // 使用中の場合

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntHeight = 0, nCntWidth = 0; nCntHeight < m_part.y; nCntHeight++)
		{ // 縦の分割数 +1回繰り返す

			for (nCntWidth = 0; nCntWidth < m_part.x + 1; nCntWidth++)
			{ // 横の分割数 +1回繰り返す

				pIdx[0] = (WORD)((m_part.x + 1) * (nCntHeight + 1) + nCntWidth);
				pIdx[1] = (WORD)((m_part.x + 1) * nCntHeight + nCntWidth);

				// インデックスデータのポインタを 2つ分進める
				pIdx += 2;
			}

			if (nCntHeight != m_part.y - 1)
			{ // 一番手前の分割場所ではない場合

				pIdx[0] = (WORD)((m_part.x + 1) * nCntHeight + nCntWidth - 1);
				pIdx[1] = (WORD)((m_part.x + 1) * (nCntHeight + 2));

				// インデックスデータのポインタを 2つ分進める
				pIdx += 2;
			}
		}

		// インデックスバッファをアンロックする
		m_pIdxBuff->Unlock();
	}
}

//============================================================
//	スクロールのテクスチャ座標の設定処理
//============================================================
void CObjectMeshWall::SetScrollTex(const float fTexU, const float fTexV)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntHeight = 0; nCntHeight < m_part.y + 1; nCntHeight++)
		{ // 縦の分割数 +1回繰り返す

			for (int nCntWidth = 0; nCntWidth < m_part.x + 1; nCntWidth++)
			{ // 横の分割数 +1回繰り返す

				// テクスチャ座標の設定
				pVtx[0].tex = D3DXVECTOR2((float)nCntWidth + fTexU, (float)nCntHeight + fTexV);

				// 頂点データのポインタを 1つ分進める
				pVtx += 1;
			}
		}

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}

//============================================================
//	通常描画処理
//============================================================
void CObjectMeshWall::DrawNormal(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive
	( // 引数
		D3DPT_TRIANGLESTRIP,	// プリミティブの種類
		0,
		0,
		m_nNumVtx,		// 使用する頂点数
		0,				// インデックスバッファの開始地点
		m_nNumIdx - 2	// プリミティブ (ポリゴン) 数
	);
}

//============================================================
//	シェーダー描画処理
//============================================================
void CObjectMeshWall::DrawShader(CShader *pShader)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 描画開始
	pShader->Begin();
	pShader->BeginPass(0);

	// マトリックス情報を設定
	pShader->SetMatrix(&m_meshWall.mtxWorld);

	// ライト方向を設定
	pShader->SetLightDirect(&m_meshWall.mtxWorld, 0);

	// 拡散光を設定
	pShader->SetOnlyDiffuse(m_meshWall.col);

	// テクスチャを設定
	pShader->SetTexture(m_nTextureID);

	// 状態変更の伝達
	pShader->CommitChanges();

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive
	( // 引数
		D3DPT_TRIANGLESTRIP,	// プリミティブの種類
		0,
		0,
		m_nNumVtx,		// 使用する頂点数
		0,				// インデックスバッファの開始地点
		m_nNumIdx - 2	// プリミティブ (ポリゴン) 数
	);

	// 描画終了
	pShader->EndPass();
	pShader->End();
}
