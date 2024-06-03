//============================================================
//
//	ブラー処理 [blur.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "blur.h"
#include "objectChara.h"
#include "objectModel.h"
#include "multiModel.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 6;	// ブラーの優先順位
}

//************************************************************
//	親クラス [CBlur] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CBlur::CBlur() : CObject(LABEL_BLUR, DIM_3D, PRIORITY),
	m_pParent		(nullptr),		// 親オブジェクト
	m_fStartAlpha	(0.0f),			// ブラーの開始透明度
	m_nMaxLength	(0),			// 保持する親オブジェクトの最大数
	m_state			(STATE_NONE),	// 状態
	m_nCounterState	(0)				// 状態管理カウンター
{
	// メンバ変数をクリア
	m_oldObject.clear();		// 過去オブジェクト情報
	m_mat = material::White();	// ブラー反映マテリアル
}

//============================================================
//	デストラクタ
//============================================================
CBlur::~CBlur()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CBlur::Init(void)
{
	// メンバ変数を初期化
	m_oldObject.clear();			// 過去オブジェクト情報
	m_mat = material::White();		// ブラー反映マテリアル
	m_pParent		= nullptr;		// 親オブジェクト
	m_fStartAlpha	= 0.0f;			// ブラーの開始透明度
	m_nMaxLength	= 0;			// 保持する親オブジェクトの最大数
	m_state			= STATE_NORMAL;	// 状態
	m_nCounterState	= 0;			// 状態管理カウンター

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CBlur::Uninit(void)
{
	for (auto& rVecObj : m_oldObject)
	{ // 保存オブジェクト数分繰り返す

		for (auto& rVecParts : rVecObj)
		{ // パーツ数分繰り返す

			// パーツの終了
			SAFE_UNINIT(rVecParts);
		}

		// パーツ格納情報をクリア
		rVecObj.clear();
	}

	// 保存オブジェクト情報をクリア
	m_oldObject.clear();

	// オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CBlur::Update(const float fDeltaTime)
{
	// 残像の生成
	if (m_state == STATE_NORMAL)
	{ // 通常状態の場合

		// オブジェクト保存用の動的配列を生成
		std::vector<CObjectModel*> tempBlur;
		tempBlur.resize((size_t)m_pParent->GetNumParts());	// パーツ数分配列生成

		int nCntParts = 0;	// パーツインデックス
		for (auto& rVecParts : tempBlur)
		{ // パーツ数分繰り返す

			// パーツの生成
			rVecParts = CObjectModel::Create(VEC3_ZERO, VEC3_ZERO);
			if (rVecParts != nullptr)
			{ // パーツの生成に成功した場合

				CObjectModel *pBlurParts = rVecParts;	// 残像パーツ
				CMultiModel *pOriginParts = m_pParent->GetParts(nCntParts);		// 原点パーツ
				D3DXMATRIX mtxParts = pOriginParts->GetMtxWorld();				// 残像生成元のマトリックス

				// 情報を生成元と一致させる
				pBlurParts->BindModel(pOriginParts->GetModelID());	// 同一モデルを割当
				pBlurParts->SetMtxWorld(mtxParts);					// 同一マトリックスを設定

				// 位置・向き・大きさをマトリックスから取得し設定
				pBlurParts->SetVec3Position(useful::GetMatrixPosition(mtxParts));
				pBlurParts->SetVec3Rotation(useful::GetMatrixRotation(mtxParts));
				pBlurParts->SetVec3Scaling(useful::GetMatrixScaling(mtxParts));

				// 加算合成にする
				pBlurParts->GetRenderState()->SetAlphaBlend(CRenderState::BLEND_ADD);
			}

			// パーツインデックスを進める
			nCntParts++;
		}

		// 配列の最後尾に残像を追加
		m_oldObject.push_back(tempBlur);
	}

	// 残像の色反映
	{
		int nCntBlur = 0;	// ブラーインデックス
		for (auto& rVecObj : m_oldObject)
		{ // 保存オブジェクト数分繰り返す

			// マテリアルの透明度を後ろになるにつれて下げていく
			D3DXMATERIAL matSet = m_mat;	// 設定マテリアル
			matSet.MatD3D.Diffuse.a = (m_fStartAlpha / m_nMaxLength) * nCntBlur;

			for (auto& rVecParts : rVecObj)
			{ // パーツ数分繰り返す

				// パーツの全マテリアルを変更
				rVecParts->SetAllMaterial(matSet);
			}

			// ブラーインデックスを進める
			nCntBlur++;
		}
	}

	// 残像の削除
	{
		bool bDelete = false;	// 削除状況
		switch (m_state)
		{ // 状態ごとの処理
		case STATE_NONE:
			break;

		case STATE_NORMAL:

			// 削除条件を指定 (残像が最大数を超えた場合削除)
			bDelete = (int)m_oldObject.size() > m_nMaxLength;
			break;

		case STATE_VANISH:

			// 削除条件を指定 (確定削除)
			bDelete = true;
			break;

		default:
			assert(false);
			break;
		}

		if (bDelete)
		{ // 削除を行う場合

			if (m_oldObject.size() <= 0)
			{ // 配列要素がない場合
			
				// 何もしない状態にする
				m_state = STATE_NONE;

				// 処理を抜ける
				return;
			}

			std::vector<CObjectModel*> objFront = m_oldObject.front();	// 残像配列の先頭
			for (auto& rVecParts : objFront)
			{ // パーツ数分繰り返す

				// パーツの終了
				SAFE_UNINIT(rVecParts);
			}

			// パーツ格納情報をクリア
			objFront.clear();

			// 残像の最後尾を配列から削除
			m_oldObject.erase(m_oldObject.begin());
		}
	}
}

//============================================================
//	描画処理
//============================================================
void CBlur::Draw(CShader *pShader)
{
	// 何もしない状態の場合抜ける
	if (m_state == STATE_NONE) { return; }

	for (auto& rVecObj : m_oldObject)
	{ // 保存オブジェクト数分繰り返す

		for (auto& rVecParts : rVecObj)
		{ // パーツ数分繰り返す

			// パーツの描画
			rVecParts->Draw(pShader);
		}
	}
}

//============================================================
//	生成処理
//============================================================
CBlur *CBlur::Create
(
	CObjectChara *pParent,		// 親オブジェクト
	D3DXMATERIAL& rMat,			// ブラーマテリアル
	const float fStartAlpha,	// ブラー開始透明度
	const int nMaxLength		// 保持オブジェクト最大数
)
{
	// ブラーの生成
	CBlur *pBlur = new CBlur;
	if (pBlur == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// ブラーの初期化
		if (FAILED(pBlur->Init()))
		{ // 初期化に失敗した場合

			// ブラーの破棄
			SAFE_DELETE(pBlur);
			return nullptr;
		}

		// 親オブジェクトを設定
		pBlur->m_pParent = pParent;

		// ブラーマテリアルを設定
		pBlur->SetMaterial(rMat);

		// ブラーの開始透明度を設定
		pBlur->SetStartAlpha(fStartAlpha);

		// 保持オブジェクトの最大数を設定
		pBlur->SetMaxLength(nMaxLength);

		// 確保したアドレスを返す
		return pBlur;
	}
}

//============================================================
//	状態の設定処理
//============================================================
void CBlur::SetState(const EState state)
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
	case STATE_VANISH:	// 消失状態
	case STATE_NORMAL:	// 通常状態
		break;

	default:	// 例外状態
		assert(false);
		break;
	}
}
