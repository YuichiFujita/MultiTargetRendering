//============================================================
//
//	オブジェクトモデル処理 [objectModel.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectModel.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	子クラス [CObjectModel] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectModel::CObjectModel(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority),
	m_pRenderState	(nullptr),		// レンダーステートの情報
	m_pMat			(nullptr),		// マテリアルへのポインタ
	m_pos			(VEC3_ZERO),	// 位置
	m_rot			(VEC3_ZERO),	// 向き
	m_scale			(VEC3_ZERO),	// 拡大率
	m_nModelID		(0)				// モデルインデックス
{
	// メンバ変数をクリア
	memset(&m_modelData, 0, sizeof(m_modelData));	// モデル情報
	D3DXMatrixIdentity(&m_mtxWorld);				// ワールドマトリックス
}

//============================================================
//	デストラクタ
//============================================================
CObjectModel::~CObjectModel()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectModel::Init(void)
{
	// メンバ変数を初期化
	memset(&m_modelData, 0, sizeof(m_modelData));	// モデル情報
	D3DXMatrixIdentity(&m_mtxWorld);				// ワールドマトリックス
	m_pRenderState	= nullptr;		// レンダーステートの情報
	m_pMat			= nullptr;		// マテリアルへのポインタ
	m_pos			= VEC3_ZERO;	// 位置
	m_rot			= VEC3_ZERO;	// 向き
	m_scale			= VEC3_ONE;		// 拡大率
	m_nModelID		= NONE_IDX;		// モデルインデックス

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
void CObjectModel::Uninit(void)
{
	// レンダーステートの破棄
	SAFE_REF_RELEASE(m_pRenderState);

	// マテリアルの破棄
	SAFE_DEL_ARRAY(m_pMat);

	// オブジェクトモデルを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectModel::Update(const float fDeltaTime)
{

}

//============================================================
//	描画処理
//============================================================
void CObjectModel::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXMATRIX   mtxScale, mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;	// 現在のマテリアル保存用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// レンダーステートを設定
	m_pRenderState->Set();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 拡大率を反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

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

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	// レンダーステートを再設定
	m_pRenderState->Reset();
}

//============================================================
//	位置の設定処理
//============================================================
void CObjectModel::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectModel::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_rot = rRot;

	// 向きの正規化
	useful::NormalizeRot(m_rot);
}

//============================================================
//	拡大率の設定処理
//============================================================
void CObjectModel::SetVec3Scaling(const D3DXVECTOR3& rScale)
{
	// 引数の拡大率を代入
	m_scale = rScale;
}

//============================================================
//	生成処理
//============================================================
CObjectModel *CObjectModel::Create(const D3DXVECTOR3& rPos, const D3DXVECTOR3& rRot, const D3DXVECTOR3& rScale)
{
	// オブジェクトモデルの生成
	CObjectModel *pModel = new CObjectModel;
	if (pModel == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクトモデルの初期化
		if (FAILED(pModel->Init()))
		{ // 初期化に失敗した場合

			// オブジェクトモデルの破棄
			SAFE_DELETE(pModel);
			return nullptr;
		}

		// 位置を設定
		pModel->SetVec3Position(rPos);

		// 向きを設定
		pModel->SetVec3Rotation(rRot);

		// 拡大率を設定
		pModel->SetVec3Scaling(rScale);

		// 確保したアドレスを返す
		return pModel;
	}
}

//============================================================
//	マテリアル設定処理
//============================================================
void CObjectModel::SetMaterial(const D3DXMATERIAL& rMat, const int nMatID)
{
	if (nMatID > NONE_IDX && nMatID < (int)m_modelData.dwNumMat)
	{ // 引数インデックスがマテリアルの最大数を超えていない場合

		// 引数インデックスのマテリアルを設定
		m_pMat[nMatID] = rMat;
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	マテリアル取得処理
//============================================================
D3DXMATERIAL CObjectModel::GetMaterial(const int nMatID) const
{
	if (nMatID > NONE_IDX && nMatID < (int)m_modelData.dwNumMat)
	{ // 引数インデックスがマテリアルの最大数を超えていない場合

		// 引数インデックスのマテリアルを返す
		return m_pMat[nMatID];
	}
	else
	{ // 引数インデックスがマテリアルの最大数を超えている場合

		// 変数を宣言
		D3DXMATERIAL mat;	// 例外時のマテリアル

		// マテリアルのメモリをクリア
		ZeroMemory(&mat, sizeof(mat));

		// 例外処理
		assert(false);

		// 例外時のマテリアルを返す
		return mat;
	}
}

//============================================================
//	透明度の設定処理
//============================================================
void CObjectModel::SetAlpha(const float fAlpha)
{
	// マテリアルデータへのポインタを取得
	D3DXMATERIAL *pOriginMat;	// マテリアルデータへのポインタ
	pOriginMat = (D3DXMATERIAL*)m_modelData.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		// 変数を宣言
		float fSetAlpha = fAlpha;	// 設定する透明度

		// 透明度の補正
		useful::LimitNum(fSetAlpha, 0.0f, pOriginMat[nCntMat].MatD3D.Diffuse.a);

		// 透明度を設定
		m_pMat[nCntMat].MatD3D.Diffuse.a = fSetAlpha;
	}
}

//============================================================
//	透明度取得処理
//============================================================
float CObjectModel::GetAlpha(void) const
{
	// 最も不透明な透明度を探す
	float fAlpha = 0.0f;	// 最も不透明なマテリアルの透明度
	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		if (m_pMat[nCntMat].MatD3D.Diffuse.a > fAlpha)
		{ // マテリアルの透明度がより不透明だった場合

			// 現在のマテリアルの透明度を保存
			fAlpha = m_pMat[nCntMat].MatD3D.Diffuse.a;
		}
	}

	// 全パーツ内で最も不透明だったマテリアルの透明度を返す
	return fAlpha;
}

//============================================================
//	最大透明度取得処理
//============================================================
float CObjectModel::GetMaxAlpha(void) const
{
	// マテリアルデータへのポインタを取得
	D3DXMATERIAL *pOriginMat;	// マテリアルデータへのポインタ
	pOriginMat = (D3DXMATERIAL*)m_modelData.pBuffMat->GetBufferPointer();

	float fAlpha = 0.0f;	// 最も不透明なマテリアルの透明度
	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		if (pOriginMat[nCntMat].MatD3D.Diffuse.a > fAlpha)
		{ // マテリアルの透明度がより不透明だった場合

			// 現在のマテリアルの透明度を保存
			fAlpha = pOriginMat[nCntMat].MatD3D.Diffuse.a;
		}
	}

	// 全パーツ内で最も不透明だったマテリアルの透明度を返す
	return fAlpha;
}

//============================================================
//	レンダーステート情報の取得処理
//============================================================
CRenderState *CObjectModel::GetRenderState(void)
{
	// インスタンス未使用
	assert(m_pRenderState != nullptr);

	// レンダーステートの情報を返す
	return m_pRenderState;
}

//============================================================
//	モデル割当処理 (インデックス)
//============================================================
void CObjectModel::BindModel(const int nModelID)
{
	// ポインタを宣言
	CModel *pModel = GET_MANAGER->GetModel();	// モデルへのポインタ

	if (nModelID > NONE_IDX)
	{ // モデルインデックスが使用可能な場合

		// モデルインデックスを代入
		m_nModelID = nModelID;

		// モデルを割り当て
		m_modelData = *pModel->GetInfo(nModelID);

		// 元マテリアルの設定
		if (FAILED(SetOriginMaterial(m_modelData.pBuffMat, (int)m_modelData.dwNumMat)))
		{ assert(false); }	// 設定失敗
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	モデル割当処理 (パス)
//============================================================
void CObjectModel::BindModel(const char *pModelPass)
{
	// ポインタを宣言
	CModel *pModel = GET_MANAGER->GetModel();	// モデルへのポインタ

	if (pModelPass != nullptr)
	{ // 割り当てるモデルパスが存在する場合

		// モデルインデックスを代入
		m_nModelID = pModel->Regist(pModelPass);

		// モデルを割り当て
		m_modelData = *pModel->GetInfo(m_nModelID);

		// 元マテリアルの設定
		if (FAILED(SetOriginMaterial(m_modelData.pBuffMat, (int)m_modelData.dwNumMat)))
		{ assert(false); }	// 設定失敗
	}
	else { assert(false); }	// モデルパス無し
}

//============================================================
//	マテリアルの全設定処理
//============================================================
void CObjectModel::SetAllMaterial(const D3DXMATERIAL& rMat)
{
	// 引数のマテリアルを全マテリアルに設定
	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		m_pMat[nCntMat] = rMat;
	}
}

//============================================================
//	マテリアルの再設定処理
//============================================================
void CObjectModel::ResetMaterial(void)
{
	// 死んでいる場合抜ける
	if (IsDeath()) { return; }

	// マテリアルデータへのポインタを取得
	D3DXMATERIAL *pOriginMat;	// マテリアルデータへのポインタ
	pOriginMat = (D3DXMATERIAL*)m_modelData.pBuffMat->GetBufferPointer();

	// 全マテリアルに初期マテリアルを再設定
	if (m_pMat != nullptr)
	{ // 使用されている場合

		for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
		{ // マテリアルの数分繰り返す

			m_pMat[nCntMat] = pOriginMat[nCntMat];
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	モデル情報の設定処理
//============================================================
void CObjectModel::SetModelData(const CModel::SModel& rModel)
{
	// 引数のモデル情報を代入
	m_modelData = rModel;
}

//============================================================
//	マトリックスの設定処理
//============================================================
void CObjectModel::SetMtxWorld(const D3DXMATRIX& rMtxWorld)
{
	// 引数のマトリックスを設定
	m_mtxWorld = rMtxWorld;
}

//============================================================
//	マテリアルポインタ取得処理
//============================================================
D3DXMATERIAL *CObjectModel::GetPtrMaterial(const int nID) const
{
	if (nID > NONE_IDX && nID < (int)m_modelData.dwNumMat)
	{ // 引数インデックスがマテリアルの最大数を超えていない場合

		// 引数インデックスのマテリアルを返す
		return &m_pMat[nID];
	}
	else
	{ // 引数インデックスがマテリアルの最大数を超えている場合

		// nullptrを返す
		return nullptr;
	}
}

//============================================================
//	元マテリアルの設定処理
//============================================================
HRESULT CObjectModel::SetOriginMaterial(const LPD3DXBUFFER pBuffMat, const int nNumMat)
{
	//--------------------------------------------------------
	//	メモリ開放・確保
	//--------------------------------------------------------
	// マテリアルの破棄
	SAFE_DEL_ARRAY(m_pMat);

	if (m_pMat == nullptr)
	{ // ポインタが使用されていない場合

		// マテリアル数分のメモリ確保
		m_pMat = new D3DXMATERIAL[nNumMat];

		if (m_pMat != nullptr)
		{ // 確保に成功した場合

			// メモリクリア
			memset(m_pMat, 0, sizeof(D3DXMATERIAL) * nNumMat);
		}
		else { assert(false); return E_FAIL; }	// 確保失敗
	}
	else { assert(false); return E_FAIL; }	// 使用中

	//--------------------------------------------------------
	//	マテリアル情報を設定
	//--------------------------------------------------------
	// マテリアルデータへのポインタを取得
	D3DXMATERIAL *pOriginMat;	// マテリアルデータへのポインタ
	pOriginMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < nNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		// マテリアルデータをコピー
		m_pMat[nCntMat] = pOriginMat[nCntMat];
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	通常描画処理
//============================================================
void CObjectModel::DrawNormal(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		// マテリアルの設定
		pDevice->SetMaterial(&m_pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_modelData.pTextureID[nCntMat]));

		if (m_scale != VEC3_ONE)
		{ // 拡大率が変更されている場合

			// 頂点法線の自動正規化を有効にする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		}

		// モデルの描画
		m_modelData.pMesh->DrawSubset(nCntMat);

		// 頂点法線の自動正規化を無効にする
		pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	}
}

//============================================================
//	シェーダー描画処理
//============================================================
void CObjectModel::DrawShader(CShader *pShader)
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

	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		// マテリアルの設定
		pDevice->SetMaterial(&m_pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, GET_MANAGER->GetTexture()->GetPtr(m_modelData.pTextureID[nCntMat]));

		// マテリアルを設定
		pShader->SetMaterial(m_pMat[nCntMat].MatD3D);

		// テクスチャを設定
		pShader->SetTexture(m_modelData.pTextureID[nCntMat]);

		// 状態変更の伝達
		pShader->CommitChanges();

		if (m_scale != VEC3_ONE)
		{ // 拡大率が変更されている場合

			// 頂点法線の自動正規化を有効にする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		}

		// モデルの描画
		m_modelData.pMesh->DrawSubset(nCntMat);

		// 頂点法線の自動正規化を無効にする
		pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	}

	// 描画終了
	pShader->EndPass();
	pShader->End();
}
