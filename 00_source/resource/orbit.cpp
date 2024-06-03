//============================================================
//
//	軌跡処理 [orbit.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "orbit.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "scene.h"
#include "sceneGame.h"
#include "pause.h"
#include "hitStop.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 5;	// 軌跡の優先順位
}

//************************************************************
//	子クラス [COrbit] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
COrbit::COrbit() : CObject(LABEL_NONE, DIM_3D, PRIORITY),
	m_pVtxBuff		(nullptr),		// 頂点バッファ
	m_pRenderState	(nullptr),		// レンダーステートの情報
	m_state			(STATE_NORMAL),	// 状態
	m_nNumVtx		(0),			// 必要頂点数
	m_nCounterState	(0),			// 状態管理カウンター
	m_nTextureID	(0)				// テクスチャインデックス
{
	// メンバ変数をクリア
	memset(&m_orbit, 0, sizeof(m_orbit));	// 軌跡の情報
}

//============================================================
//	デストラクタ
//============================================================
COrbit::~COrbit()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT COrbit::Init(void)
{
	// メンバ変数を初期化
	m_pVtxBuff		= nullptr;		// 頂点バッファ
	m_pRenderState	= nullptr;		// レンダーステートの情報
	m_state			= STATE_NORMAL;	// 状態
	m_nNumVtx		= 0;			// 必要頂点数
	m_nCounterState	= 0;			// 状態管理カウンター
	m_nTextureID	= NONE_IDX;		// テクスチャインデックス

	// 軌跡の情報を初期化
	m_orbit.offset = SOffset(VEC3_ZERO, VEC3_ZERO, XCOL_WHITE);	// オフセット情報
	D3DXMatrixIdentity(&m_orbit.mtxVanish);			// 消失開始時の親のマトリックス
	D3DXMatrixIdentity(&m_orbit.aMtxWorldPoint[0]);	// 両端のワールドマトリックス
	D3DXMatrixIdentity(&m_orbit.aMtxWorldPoint[1]);	// 両端のワールドマトリックス
	m_orbit.pMtxParent	= nullptr;	// 親のマトリックス
	m_orbit.pPosPoint	= nullptr;	// 各頂点座標
	m_orbit.pColPoint	= nullptr;	// 各頂点カラー
	m_orbit.nPart		= 1;		// 分割数
	m_orbit.nTexPart	= 1;		// テクスチャ分割数
	m_orbit.bAlpha		= false;	// 透明化状況
	m_orbit.bInit		= false;	// 初期化状況

	// 長さを設定
	if (FAILED(SetLength(1)))
	{ // 長さの設定に失敗した場合

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
void COrbit::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// 各頂点座標の破棄
	SAFE_DEL_ARRAY(m_orbit.pPosPoint);

	// 各頂点カラーの破棄
	SAFE_DEL_ARRAY(m_orbit.pColPoint);

	// オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void COrbit::Update(const float fDeltaTime)
{

}

//============================================================
//	描画処理
//============================================================
void COrbit::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxIdent;	// 単位マトリックス設定用
	D3DXMATRIX mtxParent;	// 親のマトリックス
	bool bUpdate = true;	// 更新状況

	// 単位マトリックスの初期化
	D3DXMatrixIdentity(&mtxIdent);

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	if (GET_MANAGER->GetScene()->GetMode() == CScene::MODE_GAME)
	{ // モードがゲームの場合

		if (CSceneGame::GetPause()->IsPause()
		||  CSceneGame::GetHitStop()->IsStop())
		{ // ポーズ中・ヒットストップ中の場合

			// 更新しない状況にする
			bUpdate = false;
		}
	}
	
	if (m_state != STATE_NONE)
	{ // 何もしない状態ではない場合

		if (bUpdate)
		{ // 更新する状況の場合

			//------------------------------------------------
			//	状態ごとの設定
			//------------------------------------------------
			switch (m_state)
			{ // 状態ごとの処理
			case STATE_NORMAL:	// 通常状態

				// 親マトリックスを設定
				mtxParent = *m_orbit.pMtxParent;

				break;

			case STATE_VANISH:	// 消失状態

				// 親マトリックスを設定
				mtxParent = m_orbit.mtxVanish;

				// カウンターを加算
				if (m_nCounterState < (m_nNumVtx / orbit::MAX_OFFSET) + 1)
				{ // カウンターが軌跡が伸び切る時間より小さい場合

					// カウンターを加算
					m_nCounterState++;
				}
				else
				{ // カウンターが軌跡が伸び切る時間以上の場合

					// カウンターを補正
					m_nCounterState = 0;

					// 状態を設定
					m_state = STATE_NONE;	// 何もしない状態
				}

				break;

			default:	// 例外状態
				assert(false);
				break;
			}

			//------------------------------------------------
			//	オフセットの初期化
			//------------------------------------------------
			for (int nCntOff = 0; nCntOff < orbit::MAX_OFFSET; nCntOff++)
			{ // オフセットの数分繰り返す

				// ワールドマトリックスの初期化
				D3DXMatrixIdentity(&m_orbit.aMtxWorldPoint[nCntOff]);

				// 位置を反映
				D3DXMatrixTranslation(&m_orbit.aMtxWorldPoint[nCntOff], m_orbit.offset.aOffset[nCntOff].x, m_orbit.offset.aOffset[nCntOff].y, m_orbit.offset.aOffset[nCntOff].z);

				// 親のマトリックスと掛け合わせる
				D3DXMatrixMultiply(&m_orbit.aMtxWorldPoint[nCntOff], &m_orbit.aMtxWorldPoint[nCntOff], &mtxParent);
			}

			//------------------------------------------------
			//	頂点座標と頂点カラーの情報をずらす
			//------------------------------------------------
			for (int nCntVtx = m_nNumVtx - 1; nCntVtx >= orbit::MAX_OFFSET; nCntVtx--)
			{ // 維持する頂点の最大数分繰り返す (オフセット分は含まない)

				// 頂点情報をずらす
				m_orbit.pPosPoint[nCntVtx] = m_orbit.pPosPoint[nCntVtx - orbit::MAX_OFFSET];
				m_orbit.pColPoint[nCntVtx] = m_orbit.pColPoint[nCntVtx - orbit::MAX_OFFSET];
			}

			//------------------------------------------------
			//	最新の頂点座標と頂点カラーの情報を設定
			//------------------------------------------------
			for (int nCntOff = 0; nCntOff < orbit::MAX_OFFSET; nCntOff++)
			{ // オフセットの数分繰り返す

				// 頂点座標の設定
				m_orbit.pPosPoint[nCntOff] = D3DXVECTOR3
				( // 引数
					m_orbit.aMtxWorldPoint[nCntOff]._41,	// x
					m_orbit.aMtxWorldPoint[nCntOff]._42,	// y
					m_orbit.aMtxWorldPoint[nCntOff]._43		// z
				);

				// 頂点カラーの設定
				m_orbit.pColPoint[nCntOff] = m_orbit.offset.aCol[nCntOff];
			}
		}

		//----------------------------------------------------
		//	頂点座標と頂点カラーの情報を初期化
		//----------------------------------------------------
		if (!m_orbit.bInit)
		{ // 初期化済みではない場合

			for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
			{ // 維持する頂点の最大数分繰り返す

				// 頂点座標の設定
				m_orbit.pPosPoint[nCntVtx] = D3DXVECTOR3
				( // 引数
					m_orbit.aMtxWorldPoint[nCntVtx % orbit::MAX_OFFSET]._41,	// x
					m_orbit.aMtxWorldPoint[nCntVtx % orbit::MAX_OFFSET]._42,	// y
					m_orbit.aMtxWorldPoint[nCntVtx % orbit::MAX_OFFSET]._43		// z
				);

				// 頂点カラーの設定
				m_orbit.pColPoint[nCntVtx] = m_orbit.offset.aCol[nCntVtx % orbit::MAX_OFFSET];
			}

			// 初期化済みにする
			m_orbit.bInit = true;
		}

		//----------------------------------------------------
		//	ポリゴンの描画
		//----------------------------------------------------
		// レンダーステートを設定
		m_pRenderState->Set();

		// 頂点情報の設定
		SetVtx();

		// 単位マトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &mtxIdent);

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
}

//============================================================
//	生成処理
//============================================================
COrbit *COrbit::Create
(
	D3DXMATRIX *pMtxParent,				// 親マトリックス
	const SOffset offset,				// オフセット情報
	const int nPart,					// 分割数
	const int nTexPart,					// テクスチャ分割数
	const bool bAlpha,					// 透明化状況
	const CRenderState::EBlend blend	// αブレンド状況
)
{
	// 軌跡の生成
	COrbit *pOrbit = new COrbit;
	if (pOrbit == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 軌跡の初期化
		if (FAILED(pOrbit->Init()))
		{ // 初期化に失敗した場合

			// 軌跡の破棄
			SAFE_DELETE(pOrbit);
			return nullptr;
		}

		// 親のマトリックスを設定
		pOrbit->SetMatrixParent(pMtxParent);

		// オフセット情報を設定
		pOrbit->SetOffset(offset);

		// テクスチャ分割数を設定
		pOrbit->SetTexPart(nTexPart);

		// 透明化状況を設定
		pOrbit->SetEnableAlpha(bAlpha);

		// αブレンドを設定
		pOrbit->m_pRenderState->SetAlphaBlend(blend);

		// 長さを設定
		if (FAILED(pOrbit->SetLength(nPart)))
		{ // 長さの設定に失敗した場合

			// 軌跡の破棄
			SAFE_DELETE(pOrbit);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pOrbit;
	}
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *COrbit::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	テクスチャ割当処理 (インデックス)
//============================================================
void COrbit::BindTexture(const int nTextureID)
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
void COrbit::BindTexture(const char *pTexturePass)
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
void COrbit::DeleteMatrixParent(void)
{
	// 消失状態にする
	SetState(STATE_VANISH);

	// 親マトリックスをnullptrにする
	m_orbit.pMtxParent = nullptr;
}

//============================================================
//	状態の設定処理
//============================================================
void COrbit::SetState(const EState state)
{
	if (state == m_state && state != STATE_NORMAL)
	{ // 設定する状態が現在の状態且つ、設定する状態が通常状態の場合

		// 処理を抜ける
		return;
	}

	if (m_state == STATE_NONE && state == STATE_VANISH)
	{ // すでに消えているのに消失させようとしている場合

		// 処理を抜ける
		return;
	}

	// 引数の状態を設定
	m_state = state;

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:	// 何もしない状態

		// 無し

		break;

	case STATE_NORMAL:	// 通常状態

		// 初期化していない状態にする
		m_orbit.bInit = false;

		break;

	case STATE_VANISH:	// 消失状態

		// 現在の親マトリックスを消失するマトリックスに設定
		m_orbit.mtxVanish = *m_orbit.pMtxParent;

		break;

	default:	// 例外状態
		assert(false);
		break;
	}
}

//============================================================
//	親のマトリックスの設定処理
//============================================================
void COrbit::SetMatrixParent(D3DXMATRIX *pMtxParent)
{
	// 引数の親マトリックスを設定
	m_orbit.pMtxParent = pMtxParent;
}

//============================================================
//	オフセット情報の設定処理
//============================================================
void COrbit::SetOffset(const SOffset offset)
{
	// 引数のオフセット情報を設定
	m_orbit.offset = offset;
}

//============================================================
//	テクスチャ分割数の設定処理
//============================================================
void COrbit::SetTexPart(const int nTexPart)
{
	// 例外処理
	assert(nTexPart > 0);	// 0以下エラー

	// 引数のテクスチャ分割数を設定
	m_orbit.nTexPart = nTexPart;
}

//============================================================
//	透明化状況の設定処理
//============================================================
void COrbit::SetEnableAlpha(const bool bAlpha)
{
	// 引数の透明化状況を設定
	m_orbit.bAlpha = bAlpha;
}

//============================================================
//	初期化状況の設定処理
//============================================================
void COrbit::SetEnableInit(const bool bInit)
{
	// 引数の初期化状況を設定
	m_orbit.bInit = bInit;
}

//============================================================
//	長さの設定処理
//============================================================
HRESULT COrbit::SetLength(const int nPart)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 必要頂点数を求める
	m_nNumVtx = nPart * orbit::MAX_OFFSET;

	//--------------------------------------------------------
	//	各頂点座標の破棄・生成
	//--------------------------------------------------------
	// 各頂点座標の破棄
	SAFE_DEL_ARRAY(m_orbit.pPosPoint);
	assert(m_orbit.pPosPoint == nullptr);

	// 各頂点座標のメモリ確保
	m_orbit.pPosPoint = new D3DXVECTOR3[m_nNumVtx];
	if (m_orbit.pPosPoint != nullptr)
	{ // 確保に成功した場合

		// メモリクリア
		memset(m_orbit.pPosPoint, 0, sizeof(D3DXVECTOR3) * m_nNumVtx);
	}
	else
	{ // 確保に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	各頂点カラーの破棄・生成
	//--------------------------------------------------------
	// 各頂点カラーの破棄
	SAFE_DEL_ARRAY(m_orbit.pColPoint);
	assert(m_orbit.pColPoint == nullptr);

	// 各頂点カラーのメモリ確保
	m_orbit.pColPoint = new D3DXCOLOR[m_nNumVtx];
	if (m_orbit.pColPoint != nullptr)
	{ // 確保に成功した場合

		// メモリクリア
		memset(m_orbit.pColPoint, 0, sizeof(D3DXCOLOR) * m_nNumVtx);
	}
	else
	{ // 確保に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	頂点バッファの破棄・生成
	//--------------------------------------------------------
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

	// 頂点情報の設定
	SetVtx();

	// 成功を返す
	return S_OK;
}

//============================================================
//	頂点情報の設定処理
//============================================================
void COrbit::SetVtx(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	if (m_pVtxBuff != nullptr)
	{ // 軌跡の頂点バッファが使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
		{ // 維持する頂点の最大数分繰り返す

			// 頂点座標の設定
			pVtx[0].pos = m_orbit.pPosPoint[nCntVtx];

			// 法線ベクトルの設定
			pVtx[0].nor = VEC3_ZERO;

			// 頂点カラーの設定
			if (m_orbit.bAlpha)
			{ // 透明にしていく場合

				pVtx[0].col = D3DXCOLOR
				( // 引数
					m_orbit.pColPoint[nCntVtx].r,	// r
					m_orbit.pColPoint[nCntVtx].g,	// g
					m_orbit.pColPoint[nCntVtx].b,	// b
					m_orbit.pColPoint[nCntVtx].a - (m_orbit.pColPoint[nCntVtx].a / (m_nNumVtx * 0.5f)) * (nCntVtx / 2)	// a
				);
			}
			else
			{ // 透明度を変えない場合

				pVtx[0].col = m_orbit.pColPoint[nCntVtx];
			}

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2
			( // 引数
				(1.0f / (float)m_orbit.nTexPart) * (nCntVtx / 2),	// u
				1.0f * -((nCntVtx % 2) - 1)							// v
			);

			// 頂点データのポインタを進める
			pVtx += 1;
		}

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	通常描画処理
//============================================================
void COrbit::DrawNormal(void)
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
void COrbit::DrawShader(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX mtxIdent;	// 単位マトリックス設定用

	// 単位マトリックスの初期化
	D3DXMatrixIdentity(&mtxIdent);

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// 描画開始
	pShader->Begin();
	pShader->BeginPass(0);

	// マトリックス情報を設定
	pShader->SetMatrix(&mtxIdent);

	// ライト方向を設定
	pShader->SetLightDirect(&mtxIdent, 0);

	// 拡散光を設定
	pShader->SetOnlyDiffuse(m_orbit.pColPoint[0]);

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
