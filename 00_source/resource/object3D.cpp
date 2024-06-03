//============================================================
//
//	オブジェクト3D処理 [object3D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "object3D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "collision.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int MAX_VERTEX = 4;	// 頂点数
}

//************************************************************
//	子クラス [CObject3D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObject3D::CObject3D(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_pVtxBuff		(nullptr),			// 頂点バッファへのポインタ
	m_pRenderState	(nullptr),			// レンダーステートの情報
	m_pPosGapBuff	(nullptr),			// 座標のずれバッファ
	m_pos			(VEC3_ZERO),		// 位置
	m_rot			(VEC3_ZERO),		// 向き
	m_size			(VEC3_ZERO),		// 大きさ
	m_col			(XCOL_WHITE),		// 色
	m_origin		(ORIGIN_CENTER),	// 原点
	m_nTextureID	(0)					// テクスチャインデックス
{
	// メンバ変数をクリア
	D3DXMatrixIdentity(&m_mtxWorld);	// ワールドマトリックス
}

//============================================================
//	デストラクタ
//============================================================
CObject3D::~CObject3D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObject3D::Init(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// メンバ変数を初期化
	D3DXMatrixIdentity(&m_mtxWorld);	// ワールドマトリックス
	m_pVtxBuff		= nullptr;			// 頂点バッファへのポインタ
	m_pRenderState	= nullptr;			// レンダーステートの情報
	m_pPosGapBuff	= nullptr;			// 座標のずれバッファ
	m_pos			= VEC3_ZERO;		// 位置
	m_rot			= VEC3_ZERO;		// 向き
	m_size			= VEC3_ZERO;		// 大きさ
	m_col			= XCOL_WHITE;		// 色
	m_origin		= ORIGIN_CENTER;	// 原点
	m_nTextureID	= NONE_IDX;			// テクスチャインデックス

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

	// レンダーステートの生成
	m_pRenderState = CRenderState::Create();
	if (m_pRenderState == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 座標のずれバッファのメモリ確保
	m_pPosGapBuff = new D3DXVECTOR3[MAX_VERTEX];
	if (m_pPosGapBuff != nullptr)
	{ // 確保に成功した場合

		// メモリクリア
		memset(m_pPosGapBuff, 0, sizeof(D3DXVECTOR3) * MAX_VERTEX);
	}
	else
	{ // 生成に失敗した場合

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
void CObject3D::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// 座標のずれバッファの破棄
	SAFE_DEL_ARRAY(m_pPosGapBuff);

	// オブジェクト3Dを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObject3D::Update(const float fDeltaTime)
{

}

//============================================================
//	描画処理
//============================================================
void CObject3D::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// レンダーステートを設定
	m_pRenderState->Set();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

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

	// レンダーステートを再設定
	m_pRenderState->Reset();
}

//============================================================
//	生成処理
//============================================================
CObject3D *CObject3D::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize,	// 大きさ
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const EOrigin origin		// 原点
)
{
	// オブジェクト3Dの生成
	CObject3D *pObject3D = new CObject3D;
	if (pObject3D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクト3Dの初期化
		if (FAILED(pObject3D->Init()))
		{ // 初期化に失敗した場合

			// オブジェクト3Dの破棄
			SAFE_DELETE(pObject3D);
			return nullptr;
		}

		// 原点を設定
		pObject3D->SetOrigin(origin);

		// 位置を設定
		pObject3D->SetVec3Position(rPos);

		// 向きを設定
		pObject3D->SetVec3Rotation(rRot);

		// 大きさを設定
		pObject3D->SetVec3Sizing(rSize);

		// 色を設定
		pObject3D->SetColor(rCol);

		// 確保したアドレスを返す
		return pObject3D;
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CObject3D::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	向きの設定処理
//============================================================
void CObject3D::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_rot = rRot;

	// 向きの正規化
	useful::NormalizeRot(m_rot);

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	大きさの設定処理
//============================================================
void CObject3D::SetVec3Sizing(const D3DXVECTOR3& rSize)
{
	// 引数の大きさを代入
	m_size = rSize;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *CObject3D::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObject3D::BindTexture(const int nTextureID)
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
void CObject3D::BindTexture(const char *pTexturePass)
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
void CObject3D::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を代入
	m_col = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	原点の設定処理
//============================================================
void CObject3D::SetOrigin(const EOrigin origin)
{
	// 引数の原点を設定
	m_origin = origin;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	頂点位置の設定処理
//============================================================
void CObject3D::SetVertexPosition(const int nID, const D3DXVECTOR3& rPos)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		if (nID < MAX_VERTEX)
		{ // インデックスが使用可能な場合

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点データのポインタを引数分進める
			pVtx += nID;

			// 頂点座標の設定
			pVtx[0].pos = rPos;

			// 頂点バッファをアンロックする
			m_pVtxBuff->Unlock();
		}
	}

	// 法線の設定・正規化
	NormalizeNormal();
}

//============================================================
//	頂点位置取得処理
//============================================================
D3DXVECTOR3 CObject3D::GetVertexPosition(const int nID)
{
	// 変数を宣言
	D3DXVECTOR3 pos;	// 頂点座標の代入用

	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		if (nID < MAX_VERTEX)
		{ // インデックスが使用可能な場合

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点データのポインタを引数分進める
			pVtx += nID;

			// 頂点座標を代入
			pos = pVtx[0].pos;

			// 頂点バッファをアンロックする
			m_pVtxBuff->Unlock();
		}
	}

	// 引数のインデックスの頂点座標を返す
	return pos;
}

//============================================================
//	座標のずれの設定処理
//============================================================
void CObject3D::SetGapPosition(const int nID, const D3DXVECTOR3& rPos)
{
	if (m_pPosGapBuff != nullptr)
	{ // 使用中の場合

		if (nID < MAX_VERTEX)
		{ // インデックスが使用可能な場合

			// 頂点のずれを設定
			m_pPosGapBuff[nID] = rPos;
		}
		else { assert(false); }	// 使用不可
	}
}

//============================================================
//	座標のずれ取得処理
//============================================================
D3DXVECTOR3 CObject3D::GetGapPosition(const int nID)
{
	// 変数を宣言
	D3DXVECTOR3 pos = VEC3_ZERO;	// 頂点のずれの代入用

	if (m_pPosGapBuff != nullptr)
	{ // 使用中の場合

		if (nID < MAX_VERTEX)
		{ // インデックスが使用可能な場合

			// 頂点のずれを設定
			pos = m_pPosGapBuff[nID];
		}
		else { assert(false); }	// 使用不可
	}

	// 引数のインデックスの頂点のずれを返す
	return pos;
}

//============================================================
//	ポリゴンの着地取得処理
//============================================================
float CObject3D::GetPositionHeight(const D3DXVECTOR3&rPos)
{
	// 変数を宣言
	D3DXVECTOR3 nor;	// 法線ベクトル

	// 変数配列を宣言
	D3DXVECTOR3 aVtxPos[MAX_VERTEX];	// ポリゴンの頂点座標 [※] 0：左上 1：右上 2：左下 3：右下

	for (int nCnt = 0; nCnt < MAX_VERTEX; nCnt++)
	{ // 頂点数分繰り返す

		// 変数を宣言
		D3DXMATRIX mtxWorld, mtxRot, mtxTrans;		// 計算用マトリックス
		D3DXVECTOR3 pos = GetVertexPosition(nCnt);	// 頂点座標
		
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtxWorld);

		// 頂点位置を反映
		D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

		// ポリゴン向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

		// ポリゴン位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

		// 計算したマトリックスから座標を設定
		aVtxPos[nCnt] = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
	}

	if (collision::TriangleOuterPillar(aVtxPos[0], aVtxPos[1], aVtxPos[2], rPos))
	{ // ポリゴンの範囲内にいる場合

		// 法線を求める
		useful::NormalizeNormal(aVtxPos[1], aVtxPos[0], aVtxPos[2], nor);

		if (nor.y != 0.0f)
		{ // 法線が設定されている場合

			// プレイヤーの着地点を返す
			return (((rPos.x - aVtxPos[0].x) * nor.x + (-aVtxPos[0].y) * nor.y + (rPos.z - aVtxPos[0].z) * nor.z) * -1.0f) / nor.y;
		}
	}

	if (collision::TriangleOuterPillar(aVtxPos[3], aVtxPos[1], aVtxPos[2], rPos))
	{ // ポリゴンの範囲内にいる場合

		// 法線を求める
		useful::NormalizeNormal(aVtxPos[2], aVtxPos[3], aVtxPos[1], nor);

		if (nor.y != 0.0f)
		{ // 法線が設定されている場合

			// プレイヤーの着地点を返す
			return (((rPos.x - aVtxPos[3].x) * nor.x + (-aVtxPos[3].y) * nor.y + (rPos.z - aVtxPos[3].z) * nor.z) * -1.0f) / nor.y;
		}
	}

	// 着地範囲外の場合現在のy座標を返す
	return rPos.y;
}

//============================================================
//	頂点情報の設定処理
//============================================================
void CObject3D::SetVtx(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	switch (m_origin)
	{ // 原点ごとの処理
	case ORIGIN_CENTER:	// 中央

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-m_size.x * 0.5f,  m_size.y * 0.5f,  m_size.z * 0.5f);
		pVtx[1].pos = D3DXVECTOR3( m_size.x * 0.5f,  m_size.y * 0.5f,  m_size.z * 0.5f);
		pVtx[2].pos = D3DXVECTOR3(-m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);
		pVtx[3].pos = D3DXVECTOR3( m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);

		break;

	case ORIGIN_DOWN:	// 下

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-m_size.x * 0.5f, m_size.y,  m_size.z * 0.5f);
		pVtx[1].pos = D3DXVECTOR3( m_size.x * 0.5f, m_size.y,  m_size.z * 0.5f);
		pVtx[2].pos = D3DXVECTOR3(-m_size.x * 0.5f,     0.0f, -m_size.z * 0.5f);
		pVtx[3].pos = D3DXVECTOR3( m_size.x * 0.5f,     0.0f, -m_size.z * 0.5f);

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 頂点からのずれ量を加算
	for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
	{ // 頂点数分繰り返す

		pVtx[nCntVtx].pos += m_pPosGapBuff[nCntVtx];
	}

	// 法線の設定・正規化
	NormalizeNormal();

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//============================================================
//	通常描画処理
//============================================================
void CObject3D::DrawNormal(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//============================================================
//	シェーダー描画処理
//============================================================
void CObject3D::DrawShader(CShader *pShader)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 描画開始
	pShader->Begin();
	pShader->BeginPass(0);

	// マトリックス情報を設定
	pShader->SetMatrix(&m_mtxWorld);

	// ライト方向を設定
	pShader->SetLightDirect(&m_mtxWorld, 0);

	// 拡散光を設定
	pShader->SetOnlyDiffuse(m_col);

	// テクスチャを設定
	pShader->SetTexture(m_nTextureID);

	// 状態変更の伝達
	pShader->CommitChanges();

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// 描画終了
	pShader->EndPass();
	pShader->End();
}

//============================================================
//	法線の正規化処理
//============================================================
void CObject3D::NormalizeNormal(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 法線の正規化
	useful::NormalizeNormal
	( // 引数
		pVtx[1].pos,	// 左位置
		pVtx[0].pos,	// 中心位置
		pVtx[2].pos,	// 右位置
		pVtx[0].nor		// 法線
	);

	// 法線の正規化
	useful::NormalizeNormal
	( // 引数
		pVtx[2].pos,	// 左位置
		pVtx[3].pos,	// 中心位置
		pVtx[1].pos,	// 右位置
		pVtx[3].nor		// 法線
	);

	// 法線ベクトルの設定
	pVtx[1].nor = (pVtx[0].nor + pVtx[3].nor) / 2;
	pVtx[2].nor = (pVtx[0].nor + pVtx[3].nor) / 2;

	// 法線を正規化
	D3DXVec3Normalize(&pVtx[1].nor, &pVtx[1].nor);
	D3DXVec3Normalize(&pVtx[2].nor, &pVtx[2].nor);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
