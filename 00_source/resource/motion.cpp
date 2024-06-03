//============================================================
//
//	モーション処理 [motion.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "motion.h"
#include "multiModel.h"
#include "objectChara.h"

//************************************************************
//	親クラス [CMotion] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CMotion::CMotion(std::function<int(void)> funcGetNumParts) :
	m_funcGetNumParts(funcGetNumParts),	// パーツ数取得関数ポインタ
	m_ppModel	(nullptr),	// モデル情報
	m_pChara	(nullptr),	// オブジェクトキャラクター情報
	m_bUpdate	(true)		// 更新状況
{

}

//============================================================
//	デストラクタ
//============================================================
CMotion::~CMotion()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CMotion::Init(void)
{
	// メンバ変数をクリア
	m_ppModel	= nullptr;	// モデル情報
	m_pChara	= nullptr;	// オブジェクトキャラクター情報
	m_bUpdate	= true;		// 更新状況

	// モーションを終了状態にする
	m_info.bFinish = true;

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CMotion::Uninit(void)
{
	// モーション情報の動的配列のクリア
	ClearVector();
}

//============================================================
//	更新処理
//============================================================
void CMotion::Update(const float fDeltaTime)
{
	if (!m_bUpdate) { return; }	// 更新しない
	if (m_info.vecMotion[m_info.nType].GetNumKey() <= 0) { return; }	// キー数未設定

	if (m_blend.nFrame > 0)
	{ // ブレンドフレームが設定されている場合

		// ブレンドの更新
		UpdateBlend();
	}
	else
	{ // ブレンドフレームが設定されていない場合

		// モーションの更新
		UpdateMotion();
	}

	// 移動の更新
	UpdateMove();
}

//============================================================
//	パーツ情報の設定処理
//============================================================
void CMotion::BindPartsData(CMultiModel **ppModel)
{
	// 引数のモデル情報を設定
	m_ppModel = ppModel;
}

//============================================================
//	モーション情報全設定処理
//============================================================
void CMotion::SetAllInfo(const SInfo& rInfo)
{
	for (auto& rVec : rInfo.vecMotion)
	{ // 読み込んだモーション数分繰り返す

		// モーション情報の追加
		AddInfo(rVec);
	}
}

//============================================================
//	モーション情報の追加処理
//============================================================
void CMotion::AddInfo(const SMotion& rMotion)
{
	int nSetMotionID = m_info.GetNumMotion();	// モーションを設定する配列番号

	// 空の要素を最後尾に追加
	m_info.vecMotion.emplace_back();

	// 引数のモーション情報を設定
	m_info.vecMotion[nSetMotionID] = rMotion;

	// モーション全体フレーム数を設定
	int nSubKey = (m_info.vecMotion[nSetMotionID].bLoop) ? 0 : 1;		// ループしない場合最後のキーは含まない
	int nLoop = m_info.vecMotion[nSetMotionID].GetNumKey() - nSubKey;	// 繰り返し数を求める
	for (int nCntKey = 0; nCntKey < nLoop; nCntKey++)
	{ // キーの総数分繰り返す

		// キーのフレーム数を加算
		m_info.vecMotion[nSetMotionID].nWholeFrame += m_info.vecMotion[nSetMotionID].vecKey[nCntKey].nFrame;
	}
}

//============================================================
//	更新状況の設定処理
//============================================================
void CMotion::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	m_bUpdate = bUpdate;
}

//============================================================
//	パーツ数設定の設定処理
//============================================================
void CMotion::SetNumParts(const int nNumParts)
{
	// パーツ原点情報をクリア
	m_info.vecOriginParts.clear();

	// パーツ原点を生成
	m_info.vecOriginParts.resize((size_t)nNumParts);

	// ブレンド開始パーツ情報をクリア
	m_blend.vecParts.clear();

	// ブレンド開始パーツを生成
	m_blend.vecParts.resize((size_t)nNumParts);
}

//============================================================
//	モーション情報の動的配列のクリア処理
//============================================================
void CMotion::ClearVector(void)
{
	for (auto& rMotionInfo : m_info.vecMotion)
	{ // モーション情報の要素数分繰り返す

		for (auto& rKeyInfo : rMotionInfo.vecKey)
		{ // キー情報の要素数分繰り返す

			// パーツ情報をクリア
			rKeyInfo.vecParts.clear();
		}

		// キー情報をクリア
		rMotionInfo.vecKey.clear();
	}

	// モーション情報をクリア
	m_info.vecMotion.clear();

	// パーツ原点情報をクリア
	m_info.vecOriginParts.clear();

	// ブレンド開始パーツ情報をクリア
	m_blend.vecParts.clear();
}

//============================================================
//	設定処理
//============================================================
void CMotion::Set(const int nType, const int nBlendFrame)
{
	// 指定されたモーションが存在しない場合抜ける
	if (nType <= NONE_IDX || nType >= m_info.GetNumMotion()) { assert(false); return; }

	// 引数のモーションの種類を設定
	m_info.nType = nType;

	// 引数のブレンドフレームを設定
	m_blend.nFrame = nBlendFrame;

	// モーション情報を初期化
	m_info.nKey			 = 0;		// モーションキー番号
	m_info.nKeyCounter	 = 0;		// モーションキーカウンター
	m_info.nWholeCounter = 0;		// モーション全体カウンター
	m_info.bFinish		 = false;	// モーション終了状況

	// ブレンド情報を初期化
	m_blend.nWholeCounter = 0;		// ブレンド全体カウンター

	// パーツの位置の初期化
	if (m_blend.nFrame > 0)
	{ // ブレンドフレームが設定されている場合

		for (int nCntParts = 0; nCntParts < m_funcGetNumParts(); nCntParts++)
		{ // パーツ数分繰り返す

			// 現在位置と現在向きを保存
			m_blend.vecParts[nCntParts].pos = m_ppModel[nCntParts]->GetVec3Position();
			m_blend.vecParts[nCntParts].rot = m_ppModel[nCntParts]->GetVec3Rotation();
		}
	}
	else
	{ // ブレンドフレームが設定されていない場合

		for (int nCntParts = 0; nCntParts < m_funcGetNumParts(); nCntParts++)
		{ // パーツ数分繰り返す

			// 初期位置と初期向きを設定
			m_ppModel[nCntParts]->SetVec3Position(m_info.vecMotion[nType].vecKey[m_info.nKey].vecParts[nCntParts].pos);
			m_ppModel[nCntParts]->SetVec3Rotation(m_info.vecMotion[nType].vecKey[m_info.nKey].vecParts[nCntParts].rot);
		}
	}
}

//============================================================
//	原点位置の設定処理
//============================================================
void CMotion::SetOriginPosition(const D3DXVECTOR3& rPos, const int nParts)
{
	if (nParts > NONE_IDX && nParts < m_funcGetNumParts())
	{ // 使用可能なインデックスの場合

		// 原点位置を設定
		m_info.vecOriginParts[nParts].pos = rPos;
	}
	else { assert(false); }	// 使用不可
}

//============================================================
//	原点向きの設定処理
//============================================================
void CMotion::SetOriginRotation(const D3DXVECTOR3& rRot, const int nParts)
{
	if (nParts > NONE_IDX && nParts < m_funcGetNumParts())
	{ // 使用可能なインデックスの場合

		// 原点向きを設定
		m_info.vecOriginParts[nParts].rot = rRot;
	}
	else { assert(false); }	// 使用不可
}

//============================================================
//	種類の総数取得処理
//============================================================
int CMotion::GetNumType(void)
{
	// モーションの種類の総数を返す
	return m_info.GetNumMotion();
}

//============================================================
//	キーの総数取得処理
//============================================================
int CMotion::GetNumKey(const int nType)
{
	// 引数モーションのキーの総数を返す
	int nSubKey = (m_info.vecMotion[m_info.GetNumMotion()].bLoop) ? 0 : 1;	// ループしない場合最後のキーは含まない
	return m_info.vecMotion[nType].GetNumKey() - nSubKey;
}

//============================================================
//	キャンセル取得処理
//============================================================
bool CMotion::IsCancel(const int nType) const
{
	if (m_info.vecMotion[nType].nCancelFrame != NONE_IDX)
	{ // キャンセルフレームが設定されている場合

		// 引数モーションのキャンセル状況を返す
		return (m_info.nWholeCounter >= m_info.vecMotion[nType].nCancelFrame);
	}

	// キャンセル不可を返す
	return false;
}

//============================================================
//	コンボ取得処理
//============================================================
bool CMotion::IsCombo(const int nType) const
{
	if (m_info.vecMotion[nType].nComboFrame != NONE_IDX)
	{ // コンボフレームが設定されている場合

		// 引数モーションのコンボ状況を返す
		return (m_info.nWholeCounter >= m_info.vecMotion[nType].nComboFrame);
	}

	// コンボ不可を返す
	return false;
}

//============================================================
//	左の攻撃判定フラグの取得処理
//============================================================
bool CMotion::IsLeftWeaponCollision(void)
{
	SMotion *pMotion = &m_info.vecMotion[m_info.nType];			// 現在のモーション情報
	if (pMotion->collLeft.nMin == NONE_IDX) { return false; }	// 開始カウント未設定
	if (pMotion->collLeft.nMax == NONE_IDX) { return false; }	// 終了カウント未設定

	if (m_info.nWholeCounter >= pMotion->collLeft.nMin
	&&  m_info.nWholeCounter <= pMotion->collLeft.nMax)
	{ // カウンターが開始と終了の範囲内の場合

		return true;
	}

	return false;
}

//============================================================
//	右の攻撃判定フラグの取得処理
//============================================================
bool CMotion::IsRightWeaponCollision(void)
{
	SMotion *pMotion = &m_info.vecMotion[m_info.nType];			// 現在のモーション情報
	if (pMotion->collRight.nMin == NONE_IDX) { return false; }	// 開始カウント未設定
	if (pMotion->collRight.nMax == NONE_IDX) { return false; }	// 終了カウント未設定

	if (m_info.nWholeCounter >= pMotion->collRight.nMin
	&&  m_info.nWholeCounter <= pMotion->collRight.nMax)
	{ // カウンターが開始と終了の範囲内の場合

		return true;
	}

	return false;
}

//============================================================
//	原点位置の取得処理
//============================================================
D3DXVECTOR3 CMotion::GetOriginPosition(const int nParts)
{
	if (nParts > NONE_IDX && nParts < m_funcGetNumParts())
	{ // 使用可能なインデックスの場合

		// 原点位置を返す
		return m_info.vecOriginParts[nParts].pos;
	}

	// インデックスエラー
	assert(false);
	return VEC3_ZERO;
}

//============================================================
//	原点向きの取得処理
//============================================================
D3DXVECTOR3 CMotion::GetOriginRotation(const int nParts)
{
	if (nParts > NONE_IDX && nParts < m_funcGetNumParts())
	{ // 使用可能なインデックスの場合

		// 原点向きを返す
		return m_info.vecOriginParts[nParts].rot;
	}

	// インデックスエラー
	assert(false);
	return VEC3_ZERO;
}

//============================================================
//	生成処理
//============================================================
CMotion *CMotion::Create(CObjectChara *pChara)
{
	// モーションの生成
	CMotion *pMotion = new CMotion(std::bind(&CObjectChara::GetNumParts, pChara));
	if (pMotion == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// モーションの初期化
		if (FAILED(pMotion->Init()))
		{ // 初期化に失敗した場合

			// モーションの破棄
			SAFE_DELETE(pMotion);
			return nullptr;
		}

		// オブジェクトキャラクターを設定
		pMotion->m_pChara = pChara;

		// 確保したアドレスを返す
		return pMotion;
	}
}

//============================================================
//	破棄処理
//============================================================
void CMotion::Release(CMotion *&prMotion)
{
	// モーションの終了
	assert(prMotion != nullptr);
	prMotion->Uninit();

	// メモリ開放
	SAFE_DELETE(prMotion);
}

//============================================================
//	移動の更新処理
//============================================================
void CMotion::UpdateMove(void)
{
	if (m_pChara == nullptr) { return; }	// オブジェクトキャラクター未設定

	// 変数を宣言
	D3DXMATRIX  mtxChara	= m_pChara->GetMtxWorld();				// キャラマトリックス
	D3DXVECTOR3 posSetChara	= m_pChara->GetVec3Position();			// キャラ設定位置
	D3DXVECTOR3 posOldChara	= useful::GetMatrixPosition(mtxChara);	// キャラ過去位置
	D3DXVECTOR3 posCurChara	= VEC3_ZERO;							// キャラ現在位置

	// 移動量を求める
	float fRate = 1.0f / (float)m_info.vecMotion[m_info.nType].vecKey[m_info.nKey].nFrame;	// キーフレーム割合
	D3DXVECTOR3 moveRate = m_info.vecMotion[m_info.nType].vecKey[m_info.nKey].move * fRate;	// フレーム移動量

	if (m_info.vecMotion[m_info.nType].vecKey[m_info.nKey].nFrame > 0)
	{ // フレームが設定されている場合

		// 移動量をマトリックスに反映
		D3DXMATRIX mtxMove;	// マトリックス計算用
		D3DXMatrixTranslation(&mtxMove, moveRate.x, moveRate.y, moveRate.z);
		D3DXMatrixMultiply(&mtxChara, &mtxMove, &mtxChara);

		// 移動量を与えたマトリックスのワールド座標を求める
		posCurChara = useful::GetMatrixPosition(mtxChara);

		// 過去と現在の位置から移動量を求め、位置に与える
		posSetChara += posOldChara - posCurChara;

		// 位置を反映
		m_pChara->SetVec3Position(posSetChara);
	}
}

//============================================================
//	モーションの更新処理
//============================================================
void CMotion::UpdateMotion(void)
{
	int nType = m_info.nType;	// モーション種類
	int nKey  = m_info.nKey;	// モーションキー番号

	// 次のモーションキー番号を求める
	int nNextKey = (nKey + 1) % m_info.vecMotion[nType].GetNumKey();

	// パーツの位置の更新
	for (int nCntParts = 0; nCntParts < m_funcGetNumParts(); nCntParts++)
	{ // パーツ数分繰り返す

		// 位置・向きの差分を求める
		D3DXVECTOR3 diffPos = m_info.vecMotion[nType].vecKey[nNextKey].vecParts[nCntParts].pos - m_info.vecMotion[nType].vecKey[nKey].vecParts[nCntParts].pos;
		D3DXVECTOR3 diffRot = m_info.vecMotion[nType].vecKey[nNextKey].vecParts[nCntParts].rot - m_info.vecMotion[nType].vecKey[nKey].vecParts[nCntParts].rot;
		useful::NormalizeRot(diffRot);	// 差分向きの正規化

		// 現在のパーツの位置・向きを更新
		float fRate = (float)m_info.nKeyCounter / (float)m_info.vecMotion[nType].vecKey[nKey].nFrame;	// キーフレーム割合
		m_ppModel[nCntParts]->SetVec3Position(m_info.vecMotion[nType].vecKey[nKey].vecParts[nCntParts].pos + diffPos * fRate);
		m_ppModel[nCntParts]->SetVec3Rotation(m_info.vecMotion[nType].vecKey[nKey].vecParts[nCntParts].rot + diffRot * fRate);
	}

	// モーションの遷移の更新
	if (m_info.nKeyCounter < m_info.vecMotion[nType].vecKey[nKey].nFrame)
	{ // 現在のキーの再生が終了していない場合

		// カウンターを加算
		m_info.nKeyCounter++;
		m_info.nWholeCounter++;
	}
	else
	{ // 現在のキーの再生が終了した場合

		// 次のキーに移行
		if (m_info.vecMotion[nType].bLoop)
		{ // モーションがループする場合

			// キーカウンターを初期化
			m_info.nKeyCounter = 0;

			// キーカウントを加算
			m_info.nKey = (m_info.nKey + 1) % m_info.vecMotion[nType].GetNumKey();	// 最大値で0に戻す

			if (m_info.nKey == 0)
			{ // キーが最初に戻った場合

				// 全体カウンターを初期化
				m_info.nWholeCounter = 0;
			}
		}
		else
		{ // モーションがループしない場合

			if (m_info.nKey < m_info.vecMotion[nType].GetNumKey() - 2)
			{ // 現在のキーが最終のキーではない場合

				// キーカウンターを初期化
				m_info.nKeyCounter = 0;

				// キーカウントを加算
				m_info.nKey++;
			}
			else
			{ // 現在のキーが最終のキーの場合

				// モーションを終了状態にする
				m_info.bFinish = true;
			}
		}
	}
}

//============================================================
//	ブレンド更新処理
//============================================================
void CMotion::UpdateBlend(void)
{
	// パーツの位置の更新
	for (int nCntParts = 0; nCntParts < m_funcGetNumParts(); nCntParts++)
	{ // パーツ数分繰り返す

		// 位置・向きの差分を求める
		D3DXVECTOR3 diffPos = m_info.vecMotion[m_info.nType].vecKey[0].vecParts[nCntParts].pos - m_blend.vecParts[nCntParts].pos;
		D3DXVECTOR3 diffRot = m_info.vecMotion[m_info.nType].vecKey[0].vecParts[nCntParts].rot - m_blend.vecParts[nCntParts].rot;
		useful::NormalizeRot(diffRot);	// 差分向きの正規化

		// 現在のパーツの位置・向きを更新
		float fRate = (float)m_blend.nWholeCounter / (float)m_blend.nFrame;	// キーフレーム割合
		m_ppModel[nCntParts]->SetVec3Position(m_blend.vecParts[nCntParts].pos + diffPos * fRate);
		m_ppModel[nCntParts]->SetVec3Rotation(m_blend.vecParts[nCntParts].rot + diffRot * fRate);
	}

	// モーションの遷移の更新
	if (m_blend.nWholeCounter < m_blend.nFrame)
	{ // ブレンドの再生が終了していない場合

		// カウンターを加算
		m_blend.nWholeCounter++;
	}
	else
	{ // ブレンドの再生が終了した場合

		// ブレンド再生フレーム数を初期化
		m_blend.nFrame = 0;

		// ブレンド全体カウンターを初期化
		m_blend.nWholeCounter = 0;
	}
}
