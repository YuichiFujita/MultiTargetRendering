//============================================================
//
//	オブジェクト竜巻処理 [objectTornado.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectTornado.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int MAX_OFFSET = 2;	// オフセットの数
}

//************************************************************
//	子クラス [CObjectTornado] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectTornado::CObjectTornado() : CObject(LABEL_NONE, DIM_3D, object::DEFAULT_PRIO),
	m_pVtxBuff		(nullptr),	// 頂点バッファ
	m_pRenderState	(nullptr),	// レンダーステートの情報
	m_nNumVtx		(0),		// 必要頂点数
	m_nNumAround	(0),		// 渦の周回数
	m_nPattern		(0),		// 渦の分割数
	m_nTextureID	(0)			// テクスチャインデックス
{
	// メンバ変数をクリア
	memset(&m_tornado, 0, sizeof(m_tornado));	// 竜巻の情報
}

//============================================================
//	デストラクタ
//============================================================
CObjectTornado::~CObjectTornado()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectTornado::Init(void)
{
	// メンバ変数を初期化
	m_pVtxBuff		= nullptr;	// 頂点バッファ
	m_pRenderState	= nullptr;	// レンダーステートの情報
	m_nNumVtx		= 0;		// 必要頂点数
	m_nNumAround	= 0;		// 渦の周回数
	m_nPattern		= 0;		// 渦の分割数
	m_nTextureID	= NONE_IDX;	// テクスチャインデックス

	// 竜巻の情報を初期化
	m_tornado.pos			= VEC3_ZERO;	// 位置
	m_tornado.rot			= VEC3_ZERO;	// 向き
	m_tornado.growRot		= VEC3_ZERO;	// 成長向き
	m_tornado.col			= XCOL_WHITE;	// 色
	m_tornado.pMtxParent	= nullptr;		// 親のマトリックス
	m_tornado.fMoveRot		= 0.0f;			// 向きの変更量
	m_tornado.fThickness	= 0.0f;			// ポリゴンの太さ
	m_tornado.fOuterPlusY	= 0.0f;			// ポリゴン外周のY座標加算量
	m_tornado.fSetWidth		= 0.0f;			// 生成時の横ずれ量
	m_tornado.fSetAlpha		= 0.0f;			// 生成時の透明度
	m_tornado.fAddWidth		= 0.0f;			// 横ずれの加算量
	m_tornado.fAddHeight	= 0.0f;			// 縦ずれの加算量
	m_tornado.fSubAlpha		= 0.0f;			// 透明度の減算量
	m_tornado.fGrowWidth	= 0.0f;			// 横ずれの成長量
	m_tornado.fGrowHeight	= 0.0f;			// 縦ずれの成長量
	m_tornado.fGrowAlpha	= 0.0f;			// 透明度の成長量

	// 渦を設定
	if (FAILED(SetVortex(1, 1)))
	{ // 渦の設定に失敗した場合

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
void CObjectTornado::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// オブジェクト竜巻を破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectTornado::Update(const float fDeltaTime)
{
	// 竜巻の向きを変更
	m_tornado.rot.y -= m_tornado.fMoveRot;

	// 向きを正規化
	useful::NormalizeRot(m_tornado.rot.y);

	// 竜巻を成長させる
	m_tornado.fSetWidth  += m_tornado.fGrowWidth * m_nNumVtx;	// 原点からの横ずれ量を加算
	m_tornado.fAddWidth  += m_tornado.fGrowWidth;	// 竜巻の横の広がりを加算
	m_tornado.fAddHeight += m_tornado.fGrowHeight;	// 竜巻の縦の広がりを加算
	m_tornado.fSetAlpha  -= m_tornado.fGrowAlpha;	// 竜巻の透明度を減算 (透明にしていく)

	// 頂点情報の設定
	SetVtx();

	if (m_tornado.fSetAlpha <= 0.0f)
	{ // 竜巻が透明になった場合

		// 竜巻オブジェクトの終了
		Uninit();

		// 処理を抜ける
		return;
	}
}

//============================================================
//	描画処理
//============================================================
void CObjectTornado::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxOrigin;			// 発生源のマトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// レンダーステートを設定
	m_pRenderState->Set();

	//--------------------------------------------------------
	//	発生源のマトリックスを求める
	//--------------------------------------------------------
	// 発生源のマトリックスの初期化
	D3DXMatrixIdentity(&mtxOrigin);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_tornado.growRot.y, m_tornado.growRot.x, m_tornado.growRot.z);
	D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, &mtxRot);	// 成長向き

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_tornado.pos.x, m_tornado.pos.y, m_tornado.pos.z);
	D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, &mtxTrans);	// 発生位置

	//--------------------------------------------------------
	//	ワールドマトリックスを求める
	//--------------------------------------------------------
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_tornado.mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_tornado.rot.y, m_tornado.rot.x, m_tornado.rot.z);
	D3DXMatrixMultiply(&m_tornado.mtxWorld, &m_tornado.mtxWorld, &mtxRot);	// 回転量

	//--------------------------------------------------------
	//	マトリックスを掛け合わせる
	//--------------------------------------------------------
	if (m_tornado.pMtxParent != nullptr)
	{ // 親のマトリックスが存在する場合

		// 親のマトリックスと掛け合わせる
		D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, m_tornado.pMtxParent);
	}

	// 発生源のマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_tornado.mtxWorld, &m_tornado.mtxWorld, &mtxOrigin);

	//--------------------------------------------------------
	//	竜巻を描画
	//--------------------------------------------------------
	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_tornado.mtxWorld);

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
//	位置の設定処理
//============================================================
void CObjectTornado::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_tornado.pos = rPos;
}

//============================================================
//	生成処理
//============================================================
CObjectTornado *CObjectTornado::Create
(
	const D3DXVECTOR3& rPos,		// 位置
	const D3DXVECTOR3& rGrowRot,	// 成長向き
	const D3DXCOLOR& rCol,			// 色
	D3DXMATRIX *pMtxParent,			// 親のマトリックス
	const int   nNumAround,			// 渦の周回数
	const int   nPattern,			// 渦の分割数
	const float fMoveRot,			// 向きの変更量
	const float fThickness,			// ポリゴンの太さ
	const float fOuterPlusY,		// ポリゴン外周のY座標加算量
	const float fSetWidth,			// 生成時の横ずれ量
	const float fSetAlpha,			// 生成時の透明度
	const float fAddWidth,			// 横ずれの加算量
	const float fAddHeight,			// 縦ずれの加算量
	const float fSubAlpha,			// 透明度の減算量
	const float fGrowWidth,			// 横ずれの成長量
	const float fGrowHeight,		// 縦ずれの成長量
	const float fGrowAlpha			// 透明度の成長量
)
{
	// オブジェクト竜巻の生成
	CObjectTornado *pTornado = new CObjectTornado;
	if (pTornado == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクト竜巻の初期化
		if (FAILED(pTornado->Init()))
		{ // 初期化に失敗した場合

			// オブジェクト竜巻の破棄
			SAFE_DELETE(pTornado);
			return nullptr;
		}

		// 位置を設定
		pTornado->SetVec3Position(rPos);

		// 向きを設定
		pTornado->SetRotationGrow(rGrowRot);

		// 色を設定
		pTornado->SetColor(rCol);

		// 親のマトリックスを設定
		pTornado->SetMatrixParent(pMtxParent);

		// ポリゴンの太さを設定
		pTornado->SetThickness(fThickness);

		// ポリゴン外周のY座標加算量を設定
		pTornado->SetOuterPlusY(fOuterPlusY);

		// 生成時の横ずれ量を設定
		pTornado->SetWidth(fSetWidth);

		// 生成時の透明度を設定
		pTornado->SetAlpha(fSetAlpha);

		// 横ずれの加算量を設定
		pTornado->SetAddWidth(fAddWidth);

		// 縦ずれの加算量を設定
		pTornado->SetAddHeight(fAddHeight);

		// 透明度の減算量を設定
		pTornado->SetSubAlpha(fSubAlpha);

		// 成長性を設定
		pTornado->SetGrow(fMoveRot, fGrowWidth, fGrowHeight, fGrowAlpha);

		// 渦を設定
		if (FAILED(pTornado->SetVortex(nNumAround, nPattern)))
		{ // 渦の設定に失敗した場合

			// オブジェクト竜巻の破棄
			SAFE_DELETE(pTornado);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pTornado;
	}
}

//============================================================
//	成長の設定処理
//============================================================
void CObjectTornado::SetGrow
(
	const float fMoveRot,		// 向きの変更量
	const float fGrowWidth,		// 横ずれの成長量
	const float fGrowHeight,	// 縦ずれの成長量
	const float fGrowAlpha		// 透明度の成長量
)
{
	// 引数の情報を設定
	m_tornado.fMoveRot		= fMoveRot;		// 向きの変更量
	m_tornado.fGrowWidth	= fGrowWidth;	// 横ずれの成長量
	m_tornado.fGrowHeight	= fGrowHeight;	// 縦ずれの成長量
	m_tornado.fGrowAlpha	= fGrowAlpha;	// 透明度の成長量
}

//============================================================
//	渦の設定処理
//============================================================
HRESULT CObjectTornado::SetVortex(const int nNumAround, const int nPattern)
{
	// 引数の渦を設定
	m_nNumAround = nNumAround;
	m_nPattern = nPattern;

	// 必要頂点数を求める
	m_nNumVtx = m_nNumAround * m_nPattern * MAX_OFFSET;

	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
	assert(m_pVtxBuff == nullptr);

	// 頂点バッファの生成
	if (FAILED(GET_DEVICE->CreateVertexBuffer
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

	// 頂点情報の設定
	SetVtx();

	// 成功を返す
	return S_OK;
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *CObjectTornado::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void CObjectTornado::BindTexture(const int nTextureID)
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
void CObjectTornado::BindTexture(const char *pTexturePass)
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
//	親マトリックス削除処理
//============================================================
void CObjectTornado::DeleteMatrixParent(void)
{
	// 親マトリックスをnullptrにする
	m_tornado.pMtxParent = nullptr;
}

//============================================================
//	親のマトリックスの設定処理
//============================================================
void CObjectTornado::SetMatrixParent(D3DXMATRIX *pMtxParent)
{
	// 引数の親マトリックスを設定
	m_tornado.pMtxParent = pMtxParent;
}

//============================================================
//	色の設定処理
//============================================================
void CObjectTornado::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を設定
	m_tornado.col = rCol;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	成長向きの設定処理
//============================================================
void CObjectTornado::SetRotationGrow(const D3DXVECTOR3& rRot)
{
	// 引数の成長向きを設定
	m_tornado.growRot = rRot;

	// 成長向きの正規化
	useful::NormalizeRot(m_tornado.growRot);
}

//============================================================
//	ポリゴンの太さの設定処理
//============================================================
void CObjectTornado::SetThickness(const float fThickness)
{
	// 引数のポリゴンの太さを設定
	m_tornado.fThickness = fThickness;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	ポリゴン外周のY座標加算量の設定処理
//============================================================
void CObjectTornado::SetOuterPlusY(const float fOuterPlusY)
{
	// 引数のポリゴン外周のY座標加算量を設定
	m_tornado.fOuterPlusY = fOuterPlusY;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	生成時の横ずれ量の設定処理
//============================================================
void CObjectTornado::SetWidth(const float fSetWidth)
{
	// 引数の生成時の横ずれ量を設定
	m_tornado.fSetWidth = fSetWidth;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	生成時の透明度の設定処理
//============================================================
void CObjectTornado::SetAlpha(const float fSetAlpha)
{
	// 引数の生成時の透明度を設定
	m_tornado.fSetAlpha = fSetAlpha;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	横ずれの加算量の設定処理
//============================================================
void CObjectTornado::SetAddWidth(const float fAddWidth)
{
	// 引数の横ずれの加算量を設定
	m_tornado.fAddWidth = fAddWidth;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	縦ずれの加算量の設定処理
//============================================================
void CObjectTornado::SetAddHeight(const float fAddHeight)
{
	// 引数の縦ずれの加算量を設定
	m_tornado.fAddHeight = fAddHeight;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	透明度の減算量の設定処理
//============================================================
void CObjectTornado::SetSubAlpha(const float fSubAlpha)
{
	// 引数の透明度の減算量を設定
	m_tornado.fSubAlpha = fSubAlpha;

	// 頂点情報の設定
	SetVtx();
}

//============================================================
//	頂点情報の設定処理
//============================================================
void CObjectTornado::SetVtx(void)
{
	// 変数を宣言
	D3DXVECTOR3 vecPos;	// 竜巻の頂点方向ベクトル
	float fWidth  = m_tornado.fSetWidth;	// 頂点方向の横ずれ量
	float fHeight = 0.0f;					// 頂点方向の縦ずれ量
	float fAlpha  = m_tornado.fSetAlpha;	// 頂点カラーの透明度

	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 竜巻の頂点バッファが使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntWidth = 0; nCntWidth < m_nNumAround; nCntWidth++)
		{ // 渦の周回の総数分繰り返す

			for (int nCntHeight = 0; nCntHeight < m_nPattern; nCntHeight++)
			{ // 渦の分割数分繰り返す

				// 頂点方向ベクトルを求める
				vecPos.x = sinf(nCntHeight * ((D3DX_PI * 2.0f) / m_nPattern));
				vecPos.y = 1.0f;
				vecPos.z = cosf(nCntHeight * ((D3DX_PI * 2.0f) / m_nPattern));

				for (int nCntSet = 0; nCntSet < MAX_OFFSET; nCntSet++)
				{ // オフセットの総数分繰り返す

					// 頂点座標の設定
					pVtx[0].pos.x = 0.0f + vecPos.x * (fWidth + (nCntSet * m_tornado.fThickness));		// x
					pVtx[0].pos.y = 0.0f + vecPos.y * (fHeight + (nCntSet * m_tornado.fOuterPlusY));	// y
					pVtx[0].pos.z = 0.0f + vecPos.z * (fWidth + (nCntSet * m_tornado.fThickness));		// z

					// 法線ベクトルの設定
					pVtx[0].nor = VEC3_ZERO;

					if (fAlpha < 0.0f)
					{ // 透明度が範囲外の場合

						// 透明度を補正
						fAlpha = 0.0f;
					}

					// 頂点カラーの設定
					pVtx[0].col = D3DXCOLOR(m_tornado.col.r, m_tornado.col.g, m_tornado.col.b, fAlpha);

					// テクスチャ座標の設定
					pVtx[0].tex = D3DXVECTOR2
					( // 引数
						1.0f,			// u
						1.0f * nCntSet	// v
					);

					// 頂点データのポインタを進める
					pVtx += 1;
				}

				// 縦横を広げる
				fWidth  += m_tornado.fAddWidth;
				fHeight += m_tornado.fAddHeight;

				// α値を減算
				fAlpha -= m_tornado.fSubAlpha;
			}
		}

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	通常描画処理
//============================================================
void CObjectTornado::DrawNormal(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_nNumVtx - 2);
}

//============================================================
//	シェーダー描画処理
//============================================================
void CObjectTornado::DrawShader(CShader *pShader)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 描画開始
	pShader->Begin();
	pShader->BeginPass(0);

	// マトリックス情報を設定
	pShader->SetMatrix(&m_tornado.mtxWorld);

	// ライト方向を設定
	pShader->SetLightDirect(&m_tornado.mtxWorld, 0);

	// 拡散光を設定
	pShader->SetOnlyDiffuse(m_tornado.col);

	// テクスチャを設定
	pShader->SetTexture(m_nTextureID);

	// 状態変更の伝達
	pShader->CommitChanges();

	// テクスチャの設定
	pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_nTextureID));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_nNumVtx - 2);

	// 描画終了
	pShader->EndPass();
	pShader->End();
}
