//============================================================
//
//	オブジェクト分割キャラクター処理 [objectDivChara.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectDivChara.h"
#include "manager.h"
#include "renderer.h"
#include "objectChara.h"
#include "multiModel.h"

//************************************************************
//	子クラス [CObjectDivChara] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectDivChara::CObjectDivChara(const CObject::ELabel label, const CObject::EDim dimension, const int nPriority) : CObject(label, dimension, nPriority), m_nUpperParentID(0)
{
	// メンバ変数をクリア
	memset(&m_apBody[0], 0, sizeof(m_apBody));	// 身体の情報
}

//============================================================
//	デストラクタ
//============================================================
CObjectDivChara::~CObjectDivChara()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectDivChara::Init(void)
{
	// メンバ変数を初期化
	memset(&m_apBody[0], 0, sizeof(m_apBody));	// 身体の情報
	m_nUpperParentID = 0;	// 上半身親インデックス

	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		// オブジェクトキャラクターの生成
		m_apBody[i] = CObjectChara::Create(VEC3_ZERO);
		if (m_apBody[i] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 自動描画をOFFにする
		m_apBody[i]->SetEnableDraw(false);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CObjectDivChara::Uninit(void)
{
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		// オブジェクトキャラクターの終了
		m_apBody[i]->Uninit();
	}

	// オブジェクト分割キャラクターを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectDivChara::Update(const float fDeltaTime)
{
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		// オブジェクトキャラクターの更新
		m_apBody[i]->Update(fDeltaTime);
	}
}

//============================================================
//	描画処理
//============================================================
void CObjectDivChara::Draw(CShader *pShader)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetVec3Position();	// 下半身の位置
	D3DXVECTOR3 rot = GetVec3Rotation();	// 下半身の向き
	D3DXMATRIX  mtxRot, mtxTrans, mtxWorld;	// 計算用マトリックス

	// 変数配列を宣言
	D3DXMATRIX *apMtxWorld[] = { &mtxWorld, GetMultiModel(BODY_LOWER, m_nUpperParentID)->GetPtrMtxWorld() };
	static_assert(NUM_ARRAY(apMtxWorld) == BODY_MAX, "ERROR : Body Count Mismatch");

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	// 下半身のワールドマトリックスの反映
	m_apBody[BODY_LOWER]->SetMtxWorld(mtxWorld);

	for (int nCntChara = 0; nCntChara < BODY_MAX; nCntChara++)
	{ // 分割した身体の数分繰り返す

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, apMtxWorld[nCntChara]);

		for (int nCntParts = 0; nCntParts < m_apBody[nCntChara]->GetNumParts(); nCntParts++)
		{ // パーツの総数分繰り返す

			// パーツの描画
			GetMultiModel((EBody)nCntChara, nCntParts)->Draw(pShader);
		}
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CObjectDivChara::SetVec3Position(const D3DXVECTOR3 &rPos)
{
	// 引数の位置を設定
	m_apBody[BODY_LOWER]->SetVec3Position(rPos);
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectDivChara::SetVec3Rotation(const D3DXVECTOR3 &rRot)
{
	// 引数の向きを設定
	m_apBody[BODY_LOWER]->SetVec3Rotation(rRot);
}

//============================================================
//	更新状況の設定処理
//============================================================
void CObjectDivChara::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);	// 自身
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		m_apBody[i]->SetEnableUpdate(bUpdate);	// 上・下半身
	}
}

//============================================================
//	描画状況の設定処理
//============================================================
void CObjectDivChara::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);	// 自身
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		m_apBody[i]->SetEnableDraw(bDraw);	// 上・下半身
	}
}

//============================================================
//	生成処理
//============================================================
CObjectDivChara *CObjectDivChara::Create
(
	const D3DXVECTOR3 &rPos,	// 位置
	const D3DXVECTOR3 &rRot,	// 向き
	const int nUpperParentID	// 上半身親インデックス
)
{
	// オブジェクト分割キャラクターの生成
	CObjectDivChara *pObjectDivChara = new CObjectDivChara;
	if (pObjectDivChara == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// オブジェクト分割キャラクターの初期化
		if (FAILED(pObjectDivChara->Init()))
		{ // 初期化に失敗した場合

			// オブジェクト分割キャラクターの破棄
			SAFE_DELETE(pObjectDivChara);
			return nullptr;
		}

		// 上半身の親インデックスを設定
		pObjectDivChara->SetUpperParentID(nUpperParentID);

		// 位置を設定
		pObjectDivChara->SetVec3Position(rPos);

		// 向きを設定
		pObjectDivChara->SetVec3Rotation(rRot);

		// 確保したアドレスを返す
		return pObjectDivChara;
	}
}

//============================================================
//	モーションの設定処理
//============================================================
void CObjectDivChara::SetMotion
(
	const EBody bodyID,		// 身体インデックス
	const int nType,		// モーション種類
	const int nBlendFrame	// ブレンドフレーム
)
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションを設定
		m_apBody[bodyID]->SetMotion(nType, nBlendFrame);
	}
	else { assert(false); }	// インデックスエラー
}

//============================================================
//	パーツ情報の設定処理
//============================================================
void CObjectDivChara::SetPartsInfo
(
	const EBody bodyID,			// 身体インデックス
	const int nID,				// パーツインデックス
	const int nParentID,		// 親インデックス
	const D3DXVECTOR3 &rPos,	// 位置
	const D3DXVECTOR3 &rRot,	// 向き
	const char *pFileName		// ファイル名
)
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのパーツ情報を設定
		m_apBody[bodyID]->SetPartsInfo
		( // 引数
			nID,		// パーツインデックス
			nParentID,	// 親インデックス
			rPos,		// 位置
			rRot,		// 向き
			pFileName	// ファイル名
		);
	}
	else { assert(false); }	// インデックスエラー
}

//============================================================
//	マテリアルの設定処理
//============================================================
void CObjectDivChara::SetMaterial
(
	const D3DXMATERIAL &rMat,	// 設定マテリアル
	const EBody bodyID,			// 身体インデックス
	const int nPartsID,			// パーツインデックス
	const int nMatID			// マテリアルインデックス
)
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのマテリアルを設定
		m_apBody[bodyID]->SetMaterial(rMat, nPartsID, nMatID);
	}
	else { assert(false); }	// インデックスエラー
}

//============================================================
//	キャラクター情報割当
//============================================================
void CObjectDivChara::BindCharaData
(
	const char *pCharaPassLower,	// 下半身キャラ情報パス
	const char *pCharaPassUpper		// 上半身キャラ情報パス
)
{
	// 下半身のキャラクター情報の割当
	if (pCharaPassLower == nullptr) { assert(false); return; }	// パス指定なし
	m_apBody[BODY_LOWER]->BindCharaData(pCharaPassLower);

	// 上半身のキャラクター情報の割当
	if (pCharaPassUpper == nullptr) { assert(false); return; }	// パス指定なし
	m_apBody[BODY_UPPER]->BindCharaData(pCharaPassUpper);
}

//============================================================
//	モーションの更新状況の設定処理
//============================================================
void CObjectDivChara::SetEnableMotionUpdate(const bool bUpdate)
{
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		// モーションの更新状況を設定
		m_apBody[i]->SetEnableMotionUpdate(bUpdate);
	}
}

//============================================================
//	モーション情報の追加処理
//============================================================
void CObjectDivChara::AddMotionInfo(const EBody bodyID, const CMotion::SMotion& rInfo)
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーション情報を追加
		m_apBody[bodyID]->AddMotionInfo(rInfo);
	}
	else { assert(false); }	// インデックスエラー
}

//============================================================
//	モーション種類取得処理
//============================================================
int CObjectDivChara::GetMotionType(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーション種類を返す
		return m_apBody[bodyID]->GetMotionType();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーション種類の総数取得処理
//============================================================
int CObjectDivChara::GetMotionNumType(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーション種類の総数を返す
		return m_apBody[bodyID]->GetMotionNumType();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーションキー番号取得処理
//============================================================
int CObjectDivChara::GetMotionKey(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションキー番号を返す
		return m_apBody[bodyID]->GetMotionKey();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーションキーの総数取得処理
//============================================================
int CObjectDivChara::GetMotionNumKey(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションキーの総数を返す
		return m_apBody[bodyID]->GetMotionNumKey();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーションキーカウンター取得処理
//============================================================
int CObjectDivChara::GetMotionKeyCounter(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションキーカウンターを返す
		return m_apBody[bodyID]->GetMotionKeyCounter();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーション全体カウンター取得処理
//============================================================
int CObjectDivChara::GetMotionWholeCounter(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーション全体カウンターを返す
		return m_apBody[bodyID]->GetMotionWholeCounter();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーション全体フレーム数取得処理
//============================================================
int CObjectDivChara::GetMotionWholeFrame(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーション全体フレーム数を返す
		return m_apBody[bodyID]->GetMotionWholeFrame();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーションキャンセルフレーム取得処理
//============================================================
int CObjectDivChara::GetMotionCancelFrame(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションキャンセルフレームを返す
		return m_apBody[bodyID]->GetMotionCancelFrame();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーションコンボフレーム取得処理
//============================================================
int CObjectDivChara::GetMotionComboFrame(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションコンボフレームを返す
		return m_apBody[bodyID]->GetMotionComboFrame();
	}

	// インデックスエラー
	assert(false);
	return NONE_IDX;
}

//============================================================
//	モーション終了状況の取得処理
//============================================================
bool CObjectDivChara::IsMotionFinish(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーション終了状況を返す
		return m_apBody[bodyID]->IsMotionFinish();
	}

	// インデックスエラー
	assert(false);
	return false;
}

//============================================================
//	モーションループ状況の取得処理
//============================================================
bool CObjectDivChara::IsMotionLoop(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションループ状況を返す
		return m_apBody[bodyID]->IsMotionLoop();
	}

	// インデックスエラー
	assert(false);
	return false;
}

//============================================================
//	モーションキャンセル取得処理
//============================================================
bool CObjectDivChara::IsMotionCancel(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションキャンセル状況を返す
		return m_apBody[bodyID]->IsMotionCancel();
	}

	// インデックスエラー
	assert(false);
	return false;
}

//============================================================
//	モーションコンボ取得処理
//============================================================
bool CObjectDivChara::IsMotionCombo(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーションコンボ状況を返す
		return m_apBody[bodyID]->IsMotionCombo();
	}

	// インデックスエラー
	assert(false);
	return false;
}

//============================================================
//	モーション武器表示の取得処理
//============================================================
bool CObjectDivChara::IsWeaponDisp(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのモーション武器表示状況を返す
		return m_apBody[bodyID]->IsWeaponDisp();
	}

	// インデックスエラー
	assert(false);
	return false;
}

//============================================================
//	左の攻撃判定フラグの取得処理
//============================================================
bool CObjectDivChara::IsLeftWeaponCollision(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスの左の攻撃判定状況を返す
		return m_apBody[bodyID]->IsLeftWeaponCollision();
	}

	// インデックスエラー
	assert(false);
	return false;
}

//============================================================
//	右の攻撃判定フラグの取得処理
//============================================================
bool CObjectDivChara::IsRightWeaponCollision(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスの右の攻撃判定状況を返す
		return m_apBody[bodyID]->IsRightWeaponCollision();
	}

	// インデックスエラー
	assert(false);
	return false;
}

//============================================================
//	パーツ位置の設定処理
//============================================================
void CObjectDivChara::SetPartsPosition(const EBody bodyID, const int nPartsID, const D3DXVECTOR3 &rPos)
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのパーツ位置を設定
		m_apBody[bodyID]->SetPartsPosition(nPartsID, rPos);
	}
	else { assert(false); }	// インデックスエラー
}

//============================================================
//	パーツ向きの設定処理
//============================================================
void CObjectDivChara::SetPartsRotation(const EBody bodyID, const int nPartsID, const D3DXVECTOR3 &rRot)
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのパーツ向きを設定
		m_apBody[bodyID]->SetPartsRotation(nPartsID, rRot);
	}
	else { assert(false); }	// インデックスエラー
}

//============================================================
//	パーツ位置取得処理
//============================================================
D3DXVECTOR3 CObjectDivChara::GetPartsPosition(const EBody bodyID, const int nPartsID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのパーツ位置を返す
		return m_apBody[bodyID]->GetPartsPosition(nPartsID);
	}

	// インデックスエラー
	assert(false);
	return VEC3_ZERO;
}

//============================================================
//	パーツ向き取得処理
//============================================================
D3DXVECTOR3 CObjectDivChara::GetPartsRotation(const EBody bodyID, const int nPartsID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスのパーツ向きを返す
		return m_apBody[bodyID]->GetPartsRotation(nPartsID);
	}

	// インデックスエラー
	assert(false);
	return VEC3_ZERO;
}

//============================================================
//	マテリアル全設定処理
//============================================================
void CObjectDivChara::SetAllMaterial(const D3DXMATERIAL &rMat)
{
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		// 引数のマテリアルを全マテリアルに設定
		m_apBody[i]->SetAllMaterial(rMat);
	}
}

//============================================================
//	マテリアル再設定処理
//============================================================
void CObjectDivChara::ResetMaterial(void)
{
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		// 全マテリアルに初期マテリアルを再設定
		m_apBody[i]->ResetMaterial();
	}
}

//============================================================
//	透明度の設定処理
//============================================================
void CObjectDivChara::SetAlpha(const float fAlpha)
{
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		// 透明度の設定
		m_apBody[i]->SetAlpha(fAlpha);
	}
}

//============================================================
//	透明度の取得処理
//============================================================
float CObjectDivChara::GetAlpha(void) const
{
	// 変数を宣言
	float fAlpha = 0.0f;	// 最も不透明なマテリアルの透明度

	// 最も不透明な透明度を探す
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		float fCurAlpha = m_apBody[i]->GetAlpha();	// 現在のモデルの透明度
		if (fCurAlpha > fAlpha)
		{ // マテリアルの透明度がより不透明だった場合

			// 現在のキャラクターの透明度を保存
			fAlpha = fCurAlpha;
		}
	}

	// 全キャラクター内で最も不透明だったマテリアルの透明度を返す
	return fAlpha;
}

//============================================================
//	最大透明度の取得処理
//============================================================
float CObjectDivChara::GetMaxAlpha(void) const
{
	// 変数を宣言
	float fAlpha = 0.0f;	// 最も不透明なマテリアルの透明度

	// 最も不透明な透明度を探す
	for (int i = 0; i < BODY_MAX; i++)
	{ // 分割した身体の数分繰り返す

		float fCurAlpha = m_apBody[i]->GetMaxAlpha();	// 現在のモデルの透明度
		if (fCurAlpha > fAlpha)
		{ // マテリアルの透明度がより不透明だった場合

			// 現在のキャラクターの透明度を保存
			fAlpha = fCurAlpha;
		}
	}

	// 全キャラクター内で最も不透明だったマテリアルの透明度を返す
	return fAlpha;
}

//============================================================
//	上半身の親インデックスの設定処理
//============================================================
void CObjectDivChara::SetUpperParentID(const int nUpperParentID)
{
	if (nUpperParentID > NONE_IDX && nUpperParentID < m_apBody[BODY_LOWER]->GetNumParts())
	{ // 正規インデックスの場合

		// 上半身の親インデックスを設定
		m_nUpperParentID = nUpperParentID;
	}
	else { assert(false); }	// インデックスエラー
}

//============================================================
//	オブジェクトキャラクター取得処理
//============================================================
CObjectChara *CObjectDivChara::GetObjectChara(const EBody bodyID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{ // 正規インデックスの場合

		// 引数インデックスの半身を返す
		return m_apBody[bodyID];
	}

	// インデックスエラー
	assert(false);
	return nullptr;
}

//============================================================
//	マルチモデル取得処理
//============================================================
CMultiModel *CObjectDivChara::GetMultiModel(const EBody bodyID, const int nModelID) const
{
	if (bodyID > NONE_IDX && bodyID < BODY_MAX)
	{
		if (nModelID > NONE_IDX && nModelID < m_apBody[bodyID]->GetNumParts())
		{ // 正規インデックスの場合

			// 引数インデックスのパーツを返す
			return m_apBody[bodyID]->GetParts(nModelID);
		}
	}

	// インデックスエラー
	assert(false);
	return nullptr;
}
