//============================================================
//
//	オブジェクトビルボード処理 [objectBillboard.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectBillboard.h"
#include "manager.h"
#include "renderer.h"
#include "object3D.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int MAX_VERTEX = 4;	// 頂点数
}

//************************************************************
//	子クラス [CObjectBillboard] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectBillboard::CObjectBillboard(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_pVtxBuff		(nullptr),			// 頂点バッファへのポインタ
	m_pRenderState	(nullptr),			// レンダーステートの情報
	m_pos			(VEC3_ZERO),		// 位置
	m_rot			(VEC3_ZERO),		// 向き
	m_size			(VEC3_ZERO),		// 大きさ
	m_col			(XCOL_WHITE),		// 色
	m_origin		(ORIGIN_CENTER),	// 原点
	m_rotate		(ROTATE_NORMAL),	// 回転
	m_fAngle		(0.0f),				// 対角線の角度
	m_fLength		(0.0f),				// 対角線の長さ
	m_nTextureID	(0)					// テクスチャインデックス
{
	// メンバ変数をクリア
	D3DXMatrixIdentity(&m_mtxWorld);	// ワールドマトリックス
}

//============================================================
//	デストラクタ
//============================================================
CObjectBillboard::~CObjectBillboard()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectBillboard::Init(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// メンバ変数を初期化
	D3DXMatrixIdentity(&m_mtxWorld);	// ワールドマトリックス
	m_pVtxBuff		= nullptr;			// 頂点バッファへのポインタ
	m_pRenderState	= nullptr;			// レンダーステートの情報
	m_pos			= VEC3_ZERO;		// 位置
	m_rot			= VEC3_ZERO;		// 向き
	m_size			= VEC3_ZERO;		// 大きさ
	m_col			= XCOL_WHITE;		// 色
	m_origin		= ORIGIN_CENTER;	// 原点
	m_rotate		= ROTATE_NORMAL;	// 回転
	m_fAngle		= 0.0f;				// 対角線の角度
	m_fLength		= 0.0f;				// 対角線の長さ
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

	// ライティングをOFFにする
	m_pRenderState->SetLighting(false);

	// 頂点情報の設定
	SetVtx();

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CObjectBillboard::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// オブジェクトビルボードを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectBillboard::Update(const float fDeltaTime)
{
	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	描画処理
//============================================================
void CObjectBillboard::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// レンダーステートを設定
	m_pRenderState->Set();

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

	switch (m_rotate)
	{ // 回転ごとの処理
	case ROTATE_NORMAL:		// 通常回転

		// 無し

		break;

	case ROTATE_LATERAL:	// 横回転

		m_mtxWorld._21 = 0.0f;
		m_mtxWorld._23 = 0.0f;
		m_mtxWorld._24 = 0.0f;

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

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
CObjectBillboard *CObjectBillboard::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize,	// 大きさ
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const EOrigin origin,		// 原点
	const ERotate rotate		// 回転
)
{
	// オブジェクトビルボードの生成
	CObjectBillboard *pBillboard = new CObjectBillboard;
	if (pBillboard == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクトビルボードの初期化
		if (FAILED(pBillboard->Init()))
		{ // 初期化に失敗した場合

			// オブジェクトビルボードの破棄
			SAFE_DELETE(pBillboard);
			return nullptr;
		}

		// 原点を設定
		pBillboard->SetOrigin(origin);

		// 位置を設定
		pBillboard->SetVec3Position(rPos);

		// 向きを設定
		pBillboard->SetVec3Rotation(rRot);

		// 大きさを設定
		pBillboard->SetVec3Sizing(rSize);

		// 色を設定
		pBillboard->SetColor(rCol);

		// 回転を設定
		pBillboard->SetRotate(rotate);

		// 確保したアドレスを返す
		return pBillboard;
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CObjectBillboard::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectBillboard::SetVec3Rotation(const D3DXVECTOR3& rRot)
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
void CObjectBillboard::SetVec3Sizing(const D3DXVECTOR3& rSize)
{
	// 引数の大きさを代入
	m_size = rSize;

	switch (m_origin)
	{ // 原点ごとの処理
	case ORIGIN_CENTER:	// 中央

		// 対角線の角度を求める
		m_fAngle = atan2f(m_size.x, m_size.y);

		// 対角線の長さを求める
		m_fLength = sqrtf(m_size.x * m_size.x + m_size.y * m_size.y) * 0.5f;

		break;

	case ORIGIN_DOWN:	// 下

		// 対角線の角度を求める
		m_fAngle = atan2f(m_size.x, m_size.y);

		// 対角線の長さを求める
		m_fLength = sqrtf(m_size.x * m_size.x + (m_size.y * 2.0f) * (m_size.y * 2.0f)) * 0.5f;

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *CObjectBillboard::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObjectBillboard::BindTexture(const int nTextureID)
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
void CObjectBillboard::BindTexture(const char *pTexturePass)
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
void CObjectBillboard::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を代入
	m_col = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	原点の設定処理
//============================================================
void CObjectBillboard::SetOrigin(const EOrigin origin)
{
	// 引数の原点を設定
	m_origin = origin;

	switch (m_origin)
	{ // 原点ごとの処理
	case ORIGIN_CENTER:	// 中央

		// 対角線の角度を求める
		m_fAngle = atan2f(m_size.x, m_size.y);

		// 対角線の長さを求める
		m_fLength = sqrtf(m_size.x * m_size.x + m_size.y * m_size.y) * 0.5f;

		break;

	case ORIGIN_DOWN:	// 下

		// 対角線の角度を求める
		m_fAngle = atan2f(m_size.x, m_size.y);

		// 対角線の長さを求める
		m_fLength = sqrtf(m_size.x * m_size.x + (m_size.y * 2.0f) * (m_size.y * 2.0f)) * 0.5f;

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	回転の設定処理
//============================================================
void CObjectBillboard::SetRotate(const ERotate rotate)
{
	// 引数の回転を設定
	m_rotate = rotate;
}

//============================================================
//	頂点情報の設定処理
//============================================================
void CObjectBillboard::SetVtx(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	switch (m_origin)
	{ // 原点ごとの処理
	case ORIGIN_CENTER:	// 中央

		// 頂点座標の設定
		pVtx[0].pos.x = 0.0f + sinf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
		pVtx[0].pos.y = 0.0f - cosf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
		pVtx[0].pos.z = 0.0f;

		pVtx[1].pos.x = 0.0f + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
		pVtx[1].pos.y = 0.0f - cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
		pVtx[1].pos.z = 0.0f;

		pVtx[2].pos.x = 0.0f + sinf(m_rot.z - m_fAngle) * m_fLength;
		pVtx[2].pos.y = 0.0f - cosf(m_rot.z - m_fAngle) * m_fLength;
		pVtx[2].pos.z = 0.0f;

		pVtx[3].pos.x = 0.0f + sinf(m_rot.z + m_fAngle) * m_fLength;
		pVtx[3].pos.y = 0.0f - cosf(m_rot.z + m_fAngle) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case ORIGIN_DOWN:	// 下

		// 頂点座標の設定
		pVtx[0].pos.x = 0.0f + sinf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
		pVtx[0].pos.y = 0.0f - cosf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
		pVtx[0].pos.z = 0.0f;

		pVtx[1].pos.x = 0.0f + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
		pVtx[1].pos.y = 0.0f - cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
		pVtx[1].pos.z = 0.0f;

		pVtx[2].pos.x = 0.0f + sinf(m_rot.z - (D3DX_PI * 0.5f)) * m_size.x;
		pVtx[2].pos.y = 0.0f - cosf(m_rot.z - (D3DX_PI * 0.5f)) * m_size.x;
		pVtx[2].pos.z = 0.0f;

		pVtx[3].pos.x = 0.0f + sinf(m_rot.z + (D3DX_PI * 0.5f)) * m_size.x;
		pVtx[3].pos.y = 0.0f - cosf(m_rot.z + (D3DX_PI * 0.5f)) * m_size.x;
		pVtx[3].pos.z = 0.0f;

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
void CObjectBillboard::DrawNormal(void)
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
void CObjectBillboard::DrawShader(CShader *pShader)
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
