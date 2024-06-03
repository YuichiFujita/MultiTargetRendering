//============================================================
//
//	オブジェクト処理 [object.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "manager.h"
#include "loading.h"

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CObject *CObject::m_apTop[DIM_MAX][object::MAX_PRIO] = {};	// 先頭のオブジェクトへのポインタ
CObject *CObject::m_apCur[DIM_MAX][object::MAX_PRIO] = {};	// 最後尾のオブジェクトへのポインタ
DWORD CObject::m_dwNextID = 0;	// 次のユニークID
int CObject::m_nNumAll = 0;		// オブジェクトの総数

//************************************************************
//	親クラス [CObject] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObject::CObject(const ELabel label, const EDim dimension, const int nPriority)
{
	if (m_apCur[dimension][nPriority] != nullptr)
	{ // 最後尾が存在する場合

		// 現在の最後尾オブジェクトの次オブジェクトを自身に設定
		m_apCur[dimension][nPriority]->m_pNext = this;

		// 前オブジェクトを設定
		m_pPrev = m_apCur[dimension][nPriority];	// 現在の最後尾オブジェクト

		// 次オブジェクトをクリア
		m_pNext = nullptr;

		// 自身の情報アドレスを最後尾に設定
		m_apCur[dimension][nPriority] = this;
	}
	else
	{ // 最後尾が存在しない場合

		// 自身の情報アドレスを先頭に設定
		m_apTop[dimension][nPriority] = this;

		// 自身の情報アドレスを最後尾に設定
		m_apCur[dimension][nPriority] = this;

		// 前オブジェクトのクリア
		m_pPrev = nullptr;

		// 次オブジェクトのクリア
		m_pNext = nullptr;
	}

	// 自身の情報を設定
	m_label		= label;		// オブジェクトラベル
	m_dimension	= dimension;	// 次元
	m_nPriority	= nPriority;	// 優先順位
	m_dwID		= m_dwNextID;	// ユニークID
	m_bUpdate	= true;			// 更新状況
	m_bDraw		= true;			// 描画状況
	m_bDeath	= false;		// 死亡フラグ

#ifdef _DEBUG

	// 自身の表示をONにする
	m_bDebugDisp = true;

#endif	// _DEBUG

	// 次のユニークIDを設定
	m_dwNextID++;

	// オブジェクトの総数を加算
	m_nNumAll++;
}

//============================================================
//	デストラクタ
//============================================================
CObject::~CObject()
{
	// オブジェクトの総数を減算
	m_nNumAll--;
}

//============================================================
//	破棄処理
//============================================================
void CObject::Release(void)
{
	if (this != nullptr)
	{ // 使用されている場合

		if (!m_bDeath)
		{ // 死亡フラグが立っていない場合

			// 死亡フラグを立てる
			m_bDeath = true;
		}
		else { assert(false); }	// 死亡済み
	}
}

//============================================================
//	ラベルの設定処理
//============================================================
void CObject::SetLabel(const ELabel label)
{
	if (label > NONE_IDX && label < LABEL_MAX)
	{ // 引数のラベルが使用可能な場合

		// 引数のラベルを設定
		m_label = label;
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	次元の設定処理
//============================================================
void CObject::SetDimension(const EDim dimension)
{
	//--------------------------------------------------------
	//	リストの再接続
	//--------------------------------------------------------
	// 前のオブジェクトをつなぎなおす
	if (m_pNext != nullptr)
	{ // 次のオブジェクトが存在する場合

		// 前のオブジェクトを変更
		m_pNext->m_pPrev = m_pPrev;
	}

	// 次のオブジェクトをつなぎなおす
	if (m_pPrev != nullptr)
	{ // 前のオブジェクトが存在する場合

		// 次のオブジェクトを変更
		m_pPrev->m_pNext = m_pNext;
	}

	// 先頭オブジェクトの変更
	if (m_apTop[m_dimension][m_nPriority] == this)
	{ // 先頭オブジェクトが破棄するオブジェクトだった場合

		// 次のオブジェクトを先頭に指定
		m_apTop[m_dimension][m_nPriority] = m_pNext;
	}

	// 最後尾オブジェクトの変更
	if (m_apCur[m_dimension][m_nPriority] == this)
	{ // 最後尾オブジェクトが破棄するオブジェクトだった場合

		// 前のオブジェクトを最後尾に指定
		m_apCur[m_dimension][m_nPriority] = m_pPrev;
	}

	//--------------------------------------------------------
	//	次元の設定・リストへの追加
	//--------------------------------------------------------
	// 引数の次元を設定
	m_dimension = dimension;

	// 自身のオブジェクトを引数の優先順位リストに変更
	if (m_apCur[dimension][m_nPriority] != nullptr)
	{ // 最後尾が存在する場合

		// 現在の最後尾オブジェクトの次オブジェクトを自身に設定
		m_apCur[dimension][m_nPriority]->m_pNext = this;

		// 前オブジェクトを設定
		m_pPrev = m_apCur[dimension][m_nPriority];	// 現在の最後尾オブジェクト

		// 次オブジェクトをクリア
		m_pNext = nullptr;

		// 自身の情報アドレスを最後尾に設定
		m_apCur[dimension][m_nPriority] = this;
	}
	else
	{ // 最後尾が存在しない場合

		// 自身の情報アドレスを先頭に設定
		m_apTop[dimension][m_nPriority] = this;

		// 自身の情報アドレスを最後尾に設定
		m_apCur[dimension][m_nPriority] = this;

		// 前オブジェクトのクリア
		m_pPrev = nullptr;

		// 次オブジェクトのクリア
		m_pNext = nullptr;
	}
}

//============================================================
//	優先順位の設定処理
//============================================================
void CObject::SetPriority(const int nPriority)
{
	//--------------------------------------------------------
	//	リストの再接続
	//--------------------------------------------------------
	// 前のオブジェクトをつなぎなおす
	if (m_pNext != nullptr)
	{ // 次のオブジェクトが存在する場合

		// 前のオブジェクトを変更
		m_pNext->m_pPrev = m_pPrev;
	}

	// 次のオブジェクトをつなぎなおす
	if (m_pPrev != nullptr)
	{ // 前のオブジェクトが存在する場合

		// 次のオブジェクトを変更
		m_pPrev->m_pNext = m_pNext;
	}

	// 先頭オブジェクトの変更
	if (m_apTop[m_dimension][m_nPriority] == this)
	{ // 先頭オブジェクトが破棄するオブジェクトだった場合

		// 次のオブジェクトを先頭に指定
		m_apTop[m_dimension][m_nPriority] = m_pNext;
	}

	// 最後尾オブジェクトの変更
	if (m_apCur[m_dimension][m_nPriority] == this)
	{ // 最後尾オブジェクトが破棄するオブジェクトだった場合

		// 前のオブジェクトを最後尾に指定
		m_apCur[m_dimension][m_nPriority] = m_pPrev;
	}

	//--------------------------------------------------------
	//	優先順位の設定・リストへの追加
	//--------------------------------------------------------
	// 引数の優先順位を設定
	m_nPriority = nPriority;

	// 自身のオブジェクトを引数の優先順位リストに変更
	if (m_apCur[m_dimension][nPriority] != nullptr)
	{ // 最後尾が存在する場合

		// 現在の最後尾オブジェクトの次オブジェクトを自身に設定
		m_apCur[m_dimension][nPriority]->m_pNext = this;

		// 前オブジェクトを設定
		m_pPrev = m_apCur[m_dimension][nPriority];	// 現在の最後尾オブジェクト

		// 次オブジェクトをクリア
		m_pNext = nullptr;

		// 自身の情報アドレスを最後尾に設定
		m_apCur[m_dimension][nPriority] = this;
	}
	else
	{ // 最後尾が存在しない場合

		// 自身の情報アドレスを先頭に設定
		m_apTop[m_dimension][nPriority] = this;

		// 自身の情報アドレスを最後尾に設定
		m_apCur[m_dimension][nPriority] = this;

		// 前オブジェクトのクリア
		m_pPrev = nullptr;

		// 次オブジェクトのクリア
		m_pNext = nullptr;
	}
}

//============================================================
//	更新状況の設定処理
//============================================================
void CObject::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	m_bUpdate = bUpdate;
}

//============================================================
//	描画状況の設定処理
//============================================================
void CObject::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	m_bDraw = bDraw;
}

//============================================================
//	二軸の位置の設定処理
//============================================================
void CObject::SetVec2Position(const D3DXVECTOR2& /*rPos*/)
{
	assert(false);
}

//============================================================
//	三軸の位置の設定処理
//============================================================
void CObject::SetVec3Position(const D3DXVECTOR3& /*rPos*/)
{
	assert(false);
}

//============================================================
//	向きの設定処理
//============================================================
void CObject::SetVec3Rotation(const D3DXVECTOR3& /*rRot*/)
{
	assert(false);
}

//============================================================
//	二軸の大きさの設定処理
//============================================================
void CObject::SetVec2Sizing(const D3DXVECTOR2& /*rSize*/)
{
	assert(false);
}

//============================================================
//	三軸の大きさの設定処理
//============================================================
void CObject::SetVec3Sizing(const D3DXVECTOR3& /*rSize*/)
{
	assert(false);
}

//============================================================
//	拡大率の設定処理
//============================================================
void CObject::SetVec3Scaling(const D3DXVECTOR3& /*rScale*/)
{
	assert(false);
}

//============================================================
//	二軸の位置取得処理
//============================================================
D3DXVECTOR2 CObject::GetVec2Position(void) const
{
	assert(false);
	return VEC2_ZERO;
}

//============================================================
//	三軸の位置取得処理
//============================================================
D3DXVECTOR3 CObject::GetVec3Position(void) const
{
	assert(false);
	return VEC3_ZERO;
}

//============================================================
//	向き取得処理
//============================================================
D3DXVECTOR3 CObject::GetVec3Rotation(void) const
{
	assert(false);
	return VEC3_ZERO;
}

//============================================================
//	二軸の大きさ取得処理
//============================================================
D3DXVECTOR2 CObject::GetVec2Sizing(void) const
{
	assert(false);
	return VEC2_ZERO;
}

//============================================================
//	三軸の大きさ取得処理
//============================================================
D3DXVECTOR3 CObject::GetVec3Sizing(void) const
{
	assert(false);
	return VEC3_ZERO;
}

//============================================================
//	拡大率取得処理
//============================================================
D3DXVECTOR3 CObject::GetVec3Scaling(void) const
{
	assert(false);
	return VEC3_ZERO;
}

//============================================================
//	マトリックスポインタ取得処理
//============================================================
D3DXMATRIX *CObject::GetPtrMtxWorld(void)
{
	assert(false);
	return nullptr;
}

//============================================================
//	マトリックス取得処理
//============================================================
D3DXMATRIX CObject::GetMtxWorld(void) const
{
	assert(false);
	D3DXMATRIX mtxIden;
	D3DXMatrixIdentity(&mtxIden);
	return mtxIden;
}

//============================================================
//	全破棄処理 (ラベル指定)
//============================================================
void CObject::ReleaseAll(const std::vector<ELabel> label)
{
	// ラベル指定がない場合抜ける
	if (label.size() <= 0) { return; }

	for (int nCntDim = 0; nCntDim < DIM_MAX; nCntDim++)
	{ // 次元の総数分繰り返す

		for (int nCntPri = 0; nCntPri < object::MAX_PRIO; nCntPri++)
		{ // 優先順位の総数分繰り返す

			// オブジェクトの先頭を代入
			CObject *pObject = m_apTop[nCntDim][nCntPri];
			while (pObject != nullptr)
			{ // オブジェクトが使用されている場合繰り返す

				// 次のオブジェクトを代入
				CObject *pObjectNext = pObject->m_pNext;

				if (pObject->m_label == LABEL_NONE)
				{ // 自動破棄しないラベルの場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

				if (pObject->m_bDeath)
				{ // 死亡している場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

				for (ELabel release : label)
				{ // 要素数分繰り返す

					if (pObject->m_label == release)
					{ // 破棄するラベルと一致した場合

						// オブジェクトの終了
						pObject->Uninit();
					}
				}

				// 次のオブジェクトへのポインタを代入
				pObject = pObjectNext;
			}
		}
	}
}

//============================================================
//	全破棄処理
//============================================================
void CObject::ReleaseAll(void)
{
	for (int nCntDim = 0; nCntDim < DIM_MAX; nCntDim++)
	{ // 次元の総数分繰り返す

		for (int nCntPri = 0; nCntPri < object::MAX_PRIO; nCntPri++)
		{ // 優先順位の総数分繰り返す

			// オブジェクトの先頭を代入
			CObject *pObject = m_apTop[nCntDim][nCntPri];
			while (pObject != nullptr)
			{ // オブジェクトが使用されている場合繰り返す

				// 次のオブジェクトを代入
				CObject *pObjectNext = pObject->m_pNext;

				if (pObject->m_label == LABEL_NONE)
				{ // 自動破棄しないラベルの場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

				if (pObject->m_bDeath)
				{ // 死亡している場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

				// オブジェクトの終了
				pObject->Uninit();

				// 次のオブジェクトへのポインタを代入
				pObject = pObjectNext;
			}
		}
	}

	// 全死亡処理
	DeathAll();
}

//============================================================
//	全更新処理
//============================================================
void CObject::UpdateAll(const float fDeltaTime)
{
	// オブジェクト数表示
	GET_MANAGER->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[オブジェクト数]：%d\n", m_nNumAll);

	for (int nCntDim = 0; nCntDim < DIM_MAX; nCntDim++)
	{ // 次元の総数分繰り返す

		for (int nCntPri = 0; nCntPri < object::MAX_PRIO; nCntPri++)
		{ // 優先順位の総数分繰り返す

			// オブジェクトの先頭を代入
			CObject *pObject = m_apTop[nCntDim][nCntPri];
			while (pObject != nullptr)
			{ // オブジェクトが使用されている場合繰り返す

				// 次のオブジェクトを代入
				CObject *pObjectNext = pObject->m_pNext;
				if (pObject->m_label == LABEL_NONE)
				{ // 自動更新しないラベルの場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

				if (!pObject->m_bUpdate
				||   pObject->m_bDeath)
				{ // 自動更新がOFF、または死亡している場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

				// オブジェクトの更新
				pObject->Update(fDeltaTime);

				// 次のオブジェクトへのポインタを代入
				pObject = pObjectNext;
			}
		}
	}

	// 全死亡処理
	DeathAll();
}

//============================================================
//	全描画処理
//============================================================
void CObject::DrawAll(void)
{
	// ポインタを宣言
	CLoading *pLoading = GET_MANAGER->GetLoading();	// ローディング

	// ロード中の場合抜ける
	assert(pLoading != nullptr);
	if (pLoading->GetState() != CLoading::LOAD_NONE) { return; }

	for (int nCntDim = 0; nCntDim < DIM_MAX; nCntDim++)
	{ // 次元の総数分繰り返す

		for (int nCntPri = 0; nCntPri < object::MAX_PRIO; nCntPri++)
		{ // 優先順位の総数分繰り返す

			// オブジェクトの先頭を代入
			CObject *pObject = m_apTop[nCntDim][nCntPri];
			while (pObject != nullptr)
			{ // オブジェクトが使用されている場合繰り返す

				// 次のオブジェクトを代入
				CObject *pObjectNext = pObject->m_pNext;

#ifdef _DEBUG

				if (!pObject->m_bDebugDisp)
				{ // 表示しない場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

#endif	// _DEBUG

				if (!pObject->m_bDraw
				||   pObject->m_bDeath)
				{ // 自動描画がOFF、または死亡している場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

				// オブジェクトの描画
				pObject->Draw();

				// 次のオブジェクトへのポインタを代入
				pObject = pObjectNext;
			}
		}
	}
}

//============================================================
//	全死亡処理
//============================================================
void CObject::DeathAll(void)
{
	for (int nCntDim = 0; nCntDim < DIM_MAX; nCntDim++)
	{ // 次元の総数分繰り返す

		for (int nCntPri = 0; nCntPri < object::MAX_PRIO; nCntPri++)
		{ // 優先順位の総数分繰り返す

			// オブジェクトの先頭を代入
			CObject *pObject = m_apTop[nCntDim][nCntPri];
			while (pObject != nullptr)
			{ // オブジェクトが使用されている場合繰り返す

				// 次のオブジェクトを代入
				CObject *pObjectNext = pObject->m_pNext;

				if (!pObject->m_bDeath)
				{ // 死亡フラグが立っていない場合

					// 次のオブジェクトへのポインタを代入
					pObject = pObjectNext;
					continue;
				}

				if (pObject != nullptr)
				{ // 使用されている場合

					// 前のオブジェクトをつなぎなおす
					if (pObject->m_pNext != nullptr)
					{ // 次のオブジェクトが存在する場合

						// 前のオブジェクトを変更
						pObject->m_pNext->m_pPrev = pObject->m_pPrev;
					}

					// 次のオブジェクトをつなぎなおす
					if (pObject->m_pPrev != nullptr)
					{ // 前のオブジェクトが存在する場合

						// 次のオブジェクトを変更
						pObject->m_pPrev->m_pNext = pObject->m_pNext;
					}

					// 先頭オブジェクトの変更
					if (m_apTop[nCntDim][pObject->m_nPriority] == pObject)
					{ // 先頭オブジェクトが破棄するオブジェクトだった場合

						// 次のオブジェクトを先頭に指定
						m_apTop[nCntDim][pObject->m_nPriority] = pObject->m_pNext;
					}

					// 最後尾オブジェクトの変更
					if (m_apCur[nCntDim][pObject->m_nPriority] == pObject)
					{ // 最後尾オブジェクトが破棄するオブジェクトだった場合

						// 前のオブジェクトを最後尾に指定
						m_apCur[nCntDim][pObject->m_nPriority] = pObject->m_pPrev;
					}

					// メモリ開放
					delete pObject;
					pObject = nullptr;
				}

				// 次のオブジェクトへのポインタを代入
				pObject = pObjectNext;
			}
		}
	}
}

//============================================================
//	全表示状況の設定処理
//============================================================
#ifdef _DEBUG

void CObject::SetEnableDebugDispAll(const bool bDisp2D, const bool bDisp3D)
{
	// 変数を宣言
	bool aDisp[DIM_MAX] = { bDisp3D, bDisp2D };	// 各次元の表示状況

	for (int nCntDim = 0; nCntDim < DIM_MAX; nCntDim++)
	{ // 次元の総数分繰り返す

		for (int nCntPri = 0; nCntPri < object::MAX_PRIO; nCntPri++)
		{ // 優先順位の総数分繰り返す

			// オブジェクトの先頭を代入
			CObject *pObject = m_apTop[nCntDim][nCntPri];
			while (pObject != nullptr)
			{ // オブジェクトが使用されている場合繰り返す

				// 次のオブジェクトを代入
				CObject *pObjectNext = pObject->m_pNext;

				// 引数の表示フラグを設定
				pObject->m_bDebugDisp = aDisp[nCntDim];

				// 次のオブジェクトへのポインタを代入
				pObject = pObjectNext;
			}
		}
	}
}

#endif	// _DEBUG
