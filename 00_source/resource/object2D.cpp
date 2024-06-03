//============================================================
//
//	オブジェクト2D処理 [object2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "object2D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int MAX_VERTEX = 4;	// 頂点数
}

//************************************************************
//	子クラス [CObject2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObject2D::CObject2D(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_pVtxBuff		(nullptr),		// 頂点バッファへのポインタ
	m_pRenderState	(nullptr),		// レンダーステートの情報
	m_pos			(VEC3_ZERO),	// 位置
	m_rot			(VEC3_ZERO),	// 向き
	m_size			(VEC3_ZERO),	// 大きさ
	m_col			(XCOL_WHITE),	// 色
	m_fAngle		(0.0f),			// 対角線の角度
	m_fLength		(0.0f),			// 対角線の長さ
	m_nTextureID	(0)				// テクスチャインデックス
{

}

//============================================================
//	デストラクタ
//============================================================
CObject2D::~CObject2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObject2D::Init(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// メンバ変数を初期化
	m_pVtxBuff		= nullptr;		// 頂点バッファへのポインタ
	m_pRenderState	= nullptr;		// レンダーステートの情報
	m_pos			= VEC3_ZERO;	// 位置
	m_rot			= VEC3_ZERO;	// 向き
	m_size			= VEC3_ZERO;	// 大きさ
	m_col			= XCOL_WHITE;	// 色
	m_fAngle		= 0.0f;			// 対角線の角度
	m_fLength		= 0.0f;			// 対角線の長さ
	m_nTextureID	= NONE_IDX;		// テクスチャインデックス

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

	// レンダーステートの生成
	m_pRenderState = CRenderState::Create();
	if (m_pRenderState == nullptr)
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
void CObject2D::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// 2Dオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObject2D::Update(const float fDeltaTime)
{
	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	描画処理
//============================================================
void CObject2D::Draw(CShader * /*pShader*/)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// レンダーステートを設定
	m_pRenderState->Set();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(object::FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// レンダーステートを再設定
	m_pRenderState->Reset();
}

//============================================================
//	生成処理
//============================================================
CObject2D *CObject2D::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize,	// 大きさ
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol		// 色
)
{
	// オブジェクト2Dの生成
	CObject2D *pObject2D = new CObject2D;
	if (pObject2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクト2Dの初期化
		if (FAILED(pObject2D->Init()))
		{ // 初期化に失敗した場合

			// オブジェクト2Dの破棄
			SAFE_DELETE(pObject2D);
			return nullptr;
		}

		// 位置を設定
		pObject2D->SetVec3Position(rPos);

		// 向きを設定
		pObject2D->SetVec3Rotation(rRot);

		// 大きさを設定
		pObject2D->SetVec3Sizing(rSize);

		// 色を設定
		pObject2D->SetColor(rCol);

		// 確保したアドレスを返す
		return pObject2D;
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CObject2D::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	向きの設定処理
//============================================================
void CObject2D::SetVec3Rotation(const D3DXVECTOR3& rRot)
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
void CObject2D::SetVec3Sizing(const D3DXVECTOR3& rSize)
{
	// 引数の大きさを代入
	m_size = rSize;

	// 対角線の角度を求める
	m_fAngle = atan2f(m_size.x, m_size.y);

	// 対角線の長さを求める
	m_fLength = sqrtf(m_size.x * m_size.x + m_size.y * m_size.y) * 0.5f;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *CObject2D::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObject2D::BindTexture(const int nTextureID)
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
void CObject2D::BindTexture(const char *pTexturePass)
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
void CObject2D::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を代入
	m_col = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	頂点情報の設定処理
//============================================================
void CObject2D::SetVtx(void)
{
	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標を設定
		pVtx[0].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
		pVtx[0].pos.z = 0.0f;

		pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
		pVtx[1].pos.z = 0.0f;

		pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
		pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;
		pVtx[2].pos.z = 0.0f;

		pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		// rhw の設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

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
}

//============================================================
//	アニメーションのテクスチャ座標の設定処理
//============================================================
void CObject2D::SetAnimTex(const int nPattern, const int nWidthPtrn, const int nHeightPtrn)
{
	// 変数を宣言
	float fWidthRate	= 1.0f / nWidthPtrn;	// 横の分割数の割合
	float fHeightRate	= 1.0f / nHeightPtrn;	// 縦の分割数の割合
	int nWidthCurrent	= nPattern % nWidthPtrn;					// 現在の横のパターン
	int nHeightCurrent	= (nPattern / nWidthPtrn) % nHeightPtrn;	// 現在の縦のパターン

	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(fWidthRate *  nWidthCurrent,		fHeightRate *  nHeightCurrent);
		pVtx[1].tex = D3DXVECTOR2(fWidthRate * (nWidthCurrent + 1),	fHeightRate *  nHeightCurrent);
		pVtx[2].tex = D3DXVECTOR2(fWidthRate *  nWidthCurrent,		fHeightRate * (nHeightCurrent + 1));
		pVtx[3].tex = D3DXVECTOR2(fWidthRate * (nWidthCurrent + 1),	fHeightRate * (nHeightCurrent + 1));

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}

//============================================================
//	スクロールのテクスチャ座標の設定処理
//============================================================
void CObject2D::SetScrollTex(const float fTexU, const float fTexV)
{
	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(fTexU,		fTexV);
		pVtx[1].tex = D3DXVECTOR2(fTexU + 1.0f,	fTexV);
		pVtx[2].tex = D3DXVECTOR2(fTexU,		fTexV + 1.0f);
		pVtx[3].tex = D3DXVECTOR2(fTexU + 1.0f,	fTexV + 1.0f);

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}
