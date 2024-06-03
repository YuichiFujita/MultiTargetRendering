//============================================================
//
//	オブジェクトメッシュリング処理 [objectMeshRing.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshRing.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const float CALC_TEXDIR[] = { -1.0f, 1.0f };	// テクスチャ方向の計算係数
	const POSGRID2 MIN_PART		= GRID2_ONE;		// 分割数の最小値
	const POSGRID2 MIN_TEXPART	= GRID2_ONE;		// テクスチャ分割数の最小値
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(CALC_TEXDIR) == CObjectMeshRing::TEXDIR_MAX, "ERROR : Array Count Mismatch");

//************************************************************
//	子クラス [CObjectMeshRing] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectMeshRing::CObjectMeshRing(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_pVtxBuff		(nullptr),		// 頂点バッファ
	m_pIdxBuff		(nullptr),		// インデックスバッファ
	m_pRenderState	(nullptr),		// レンダーステートの情報
	m_part			(GRID2_ZERO),	// 分割数
	m_texPart		(GRID2_ZERO),	// テクスチャ分割数
	m_nNumVtx		(0),			// 必要頂点数
	m_nNumIdx		(0),			// 必要インデックス数
	m_nTextureID	(0)				// テクスチャインデックス
{
	// メンバ変数をクリア
	memset(&m_meshRing, 0, sizeof(m_meshRing));	// メッシュリングの情報
}

//============================================================
//	デストラクタ
//============================================================
CObjectMeshRing::~CObjectMeshRing()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectMeshRing::Init(void)
{
	// メンバ変数を初期化
	m_pVtxBuff		= nullptr;		// 頂点バッファ
	m_pIdxBuff		= nullptr;		// インデックスバッファ
	m_pRenderState	= nullptr;		// レンダーステートの情報
	m_part			= MIN_PART;		// 分割数
	m_texPart		= GRID2_ONE;	// テクスチャ分割数
	m_nNumVtx		= 0;			// 必要頂点数
	m_nNumIdx		= 0;			// 必要インデックス数
	m_nTextureID	= NONE_IDX;		// テクスチャインデックス

	m_meshRing.pos			= VEC3_ZERO;		// 位置
	m_meshRing.rot			= VEC3_ZERO;		// 向き
	m_meshRing.col			= XCOL_WHITE;		// 色
	m_meshRing.texDir		= TEXDIR_OUTSIDE;	// テクスチャ方向
	m_meshRing.fHoleRadius	= 0.0f;				// 穴の半径
	m_meshRing.fThickness	= 0.0f;				// 太さ
	m_meshRing.fOuterPlusY	= 0.0f;				// 外周のY座標加算量

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
void CObjectMeshRing::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// インデックスバッファの破棄
	SAFE_RELEASE(m_pIdxBuff);

	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// オブジェクトメッシュリングを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectMeshRing::Update(const float fDeltaTime)
{

}

//============================================================
//	描画処理
//============================================================
void CObjectMeshRing::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// レンダーステートを設定
	m_pRenderState->Set();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_meshRing.mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_meshRing.rot.y, m_meshRing.rot.x, m_meshRing.rot.z);
	D3DXMatrixMultiply(&m_meshRing.mtxWorld, &m_meshRing.mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_meshRing.pos.x, m_meshRing.pos.y, m_meshRing.pos.z);
	D3DXMatrixMultiply(&m_meshRing.mtxWorld, &m_meshRing.mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_meshRing.mtxWorld);

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
void CObjectMeshRing::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_meshRing.pos = rPos;
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectMeshRing::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_meshRing.rot = rRot;

	// 向きの正規化
	useful::NormalizeRot(m_meshRing.rot);
}

//============================================================
//	生成処理
//============================================================
CObjectMeshRing *CObjectMeshRing::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart,		// 分割数
	const POSGRID2& rTexPart,	// テクスチャ分割数
	const float fHoleRadius,	// 穴の半径
	const float fThickness,		// 太さ
	const float fOuterPlusY		// 外周のY座標加算量
)
{
	// オブジェクトメッシュリングの生成
	CObjectMeshRing *pMeshRing = new CObjectMeshRing;
	if (pMeshRing == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクトメッシュリングの初期化
		if (FAILED(pMeshRing->Init()))
		{ // 初期化に失敗した場合

			// オブジェクトメッシュリングの破棄
			SAFE_DELETE(pMeshRing);
			return nullptr;
		}

		// 位置を設定
		pMeshRing->SetVec3Position(rPos);

		// 向きを設定
		pMeshRing->SetVec3Rotation(rRot);

		// 色を設定
		pMeshRing->SetColor(rCol);

		// 穴の半径を設定
		pMeshRing->SetHoleRadius(fHoleRadius);

		// 太さを設定
		pMeshRing->SetThickness(fThickness);

		// 外周のY座標加算量を設定
		pMeshRing->SetOuterPlusY(fOuterPlusY);

		// 分割数を設定
		if (FAILED(pMeshRing->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// オブジェクトメッシュリングの破棄
			SAFE_DELETE(pMeshRing);
			return nullptr;
		}

		// テクスチャ分割数を設定
		pMeshRing->SetTexPattern(rTexPart);

		// 確保したアドレスを返す
		return pMeshRing;
	}
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *CObjectMeshRing::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObjectMeshRing::BindTexture(const int nTextureID)
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
void CObjectMeshRing::BindTexture(const char *pTexturePass)
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
void CObjectMeshRing::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を設定
	m_meshRing.col = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	テクスチャ方向の設定処理
//============================================================
void CObjectMeshRing::SetTexDir(const ETexDir texDir)
{
	// テクスチャ方向を設定
	m_meshRing.texDir = texDir;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	穴の半径の設定処理
//============================================================
void CObjectMeshRing::SetHoleRadius(const float fHoleRadius)
{
	// 引数の穴の半径を設定
	m_meshRing.fHoleRadius = fHoleRadius;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	太さの設定処理
//============================================================
void CObjectMeshRing::SetThickness(const float fThickness)
{
	// 引数の太さを設定
	m_meshRing.fThickness = fThickness;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	ポリゴン外周のY座標加算量の設定処理
//============================================================
void CObjectMeshRing::SetOuterPlusY(const float fOuterPlusY)
{
	// 引数の外周のY座標加算量を設定
	m_meshRing.fOuterPlusY = fOuterPlusY;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	分割数の設定処理
//============================================================
HRESULT CObjectMeshRing::SetPattern(const POSGRID2& rPart)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 分割数の設定不可
	assert(rPart.x >= MIN_PART.x);
	assert(rPart.y >= MIN_PART.y);

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
//	テクスチャ分割数の設定処理
//============================================================
void CObjectMeshRing::SetTexPattern(const POSGRID2& rTexPart)
{
	if (rTexPart.x >= MIN_TEXPART.x
	&&  rTexPart.y >= MIN_TEXPART.y)
	{ // テクスチャ分割数が最低値以上の場合

		// 引数のテクスチャ分割数を設定
		m_texPart = rTexPart;

		// 頂点情報の設定
		SetVtx();
	}
	else { assert(false); }	// 最低値未満
}

//============================================================
//	頂点情報の設定処理
//============================================================
void CObjectMeshRing::SetVtx(void)
{
	// 変数を宣言
	D3DXVECTOR3 vecPos;	// 頂点位置の計算用
	D3DXVECTOR3 vecNor;	// 法線ベクトルの計算用
	float fRateWidth  = ((float)(m_texPart.x) / (float)(m_part.x));	// テクスチャ横分割数の割合
	float fRateHeight = ((float)(m_texPart.y) / (float)(m_part.y));	// テクスチャ縦分割数の割合

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

				// 頂点座標の方向を設定
				vecPos = D3DXVECTOR3
				( // 引数
					sinf(D3DXToRadian(nCntWidth * (360 / (float)m_part.x))) * ((nCntHeight * (m_meshRing.fThickness / (float)m_part.y)) + m_meshRing.fHoleRadius),	// x
					nCntHeight * (m_meshRing.fOuterPlusY / (float)m_part.y),																						// y
					cosf(D3DXToRadian(nCntWidth * (360 / (float)m_part.x))) * ((nCntHeight * (m_meshRing.fThickness / (float)m_part.y)) + m_meshRing.fHoleRadius)	// z
				);

				// 頂点座標の設定
				pVtx[0].pos = vecPos;

				// 法線ベクトルの設定
				pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = m_meshRing.col;

				// テクスチャ座標の設定
				pVtx[0].tex = D3DXVECTOR2
				( // 引数
					fRateWidth  * (nCntWidth  - m_part.x) * CALC_TEXDIR[m_meshRing.texDir],	// u
					fRateHeight * (nCntHeight - m_part.y) * -1.0f							// v
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
void CObjectMeshRing::SetIdx(void)
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

				pIdx[0] = (WORD)((m_part.x + 1) * nCntHeight + nCntWidth);
				pIdx[1] = (WORD)((m_part.x + 1) * (nCntHeight + 1) + nCntWidth);

				// インデックスデータのポインタを 2つ分進める
				pIdx += 2;
			}

			if (nCntHeight != m_part.y - 1)
			{ // 一番手前の分割場所ではない場合

				pIdx[0] = (WORD)((m_part.x + 1) * (nCntHeight + 1) + nCntWidth - 1);
				pIdx[1] = (WORD)((m_part.x + 1) * (nCntHeight + 1));

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
void CObjectMeshRing::SetScrollTex(const float fTexU, const float fTexV)
{
	// 変数を宣言
	float fRateWidth  = ((float)(m_texPart.x) / (float)(m_part.x));	// テクスチャ横分割数の割合
	float fRateHeight = ((float)(m_texPart.y) / (float)(m_part.y));	// テクスチャ縦分割数の割合

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
				pVtx[0].tex = D3DXVECTOR2
				( // 引数
					fTexU + fRateWidth  * (nCntWidth  - m_part.x) * CALC_TEXDIR[m_meshRing.texDir],	// u
					fTexV + fRateHeight * (nCntHeight - m_part.y) * -1.0f							// v
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
//	通常描画処理
//============================================================
void CObjectMeshRing::DrawNormal(void)
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
void CObjectMeshRing::DrawShader(CShader *pShader)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 描画開始
	pShader->Begin();
	pShader->BeginPass(0);

	// マトリックス情報を設定
	pShader->SetMatrix(&m_meshRing.mtxWorld);

	// ライト方向を設定
	pShader->SetLightDirect(&m_meshRing.mtxWorld, 0);

	// 拡散光を設定
	pShader->SetOnlyDiffuse(m_meshRing.col);

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
