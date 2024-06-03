//============================================================
//
//	マルチ数字処理 [multiValue.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "multiValue.h"
#include "manager.h"
#include "renderer.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 6;	// マルチ数字の優先順位
}

//************************************************************
//	子クラス [CMultiValue] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CMultiValue::CMultiValue() : CObject(CObject::LABEL_UI, CObject::DIM_2D, object::DEFAULT_PRIO),
	m_pos		(VEC3_ZERO),		// 原点位置
	m_rot		(VEC3_ZERO),		// 原点向き
	m_size		(VEC3_ZERO),		// 大きさ
	m_space		(VEC3_ZERO),		// 空白
	m_col		(XCOL_WHITE),		// 色
	m_alignX	(XALIGN_CENTER),	// 横配置
	m_alignY	(YALIGN_CENTER),	// 縦配置
	m_nNum		(0),				// 数字
	m_nMin		(0),				// 最小値
	m_nMax		(0)					// 最大値
{
	// 数字リストをクリア
	m_listValue.clear();
}

//============================================================
//	デストラクタ
//============================================================
CMultiValue::~CMultiValue()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CMultiValue::Init(void)
{
	// メンバ変数を初期化
	m_pos		= VEC3_ZERO;		// 原点位置
	m_rot		= VEC3_ZERO;		// 原点向き
	m_size		= VEC3_ZERO;		// 大きさ
	m_space		= VEC3_ZERO;		// 空白
	m_col		= XCOL_WHITE;		// 色
	m_alignX	= XALIGN_CENTER;	// 横配置
	m_alignY	= YALIGN_CENTER;	// 縦配置
	m_nNum		= 0;				// 数字
	m_nMin		= 0;				// 最小値
	m_nMax		= 0;				// 最大値

	// 数字リストを初期化
	m_listValue.clear();

	// 優先順位を設定
	SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CMultiValue::Uninit(void)
{
	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字の終了
		SAFE_UNINIT(rList);
	}

	// 数字リストをクリア
	m_listValue.clear();

	// 自身のオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CMultiValue::Update(const float fDeltaTime)
{
	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字の更新
		assert(rList != nullptr);
		rList->Update(fDeltaTime);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	描画処理
//============================================================
void CMultiValue::Draw(CShader *pShader)
{
	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字の描画
		assert(rList != nullptr);
		rList->Draw(pShader);
	}
}

//============================================================
//	優先順位の設定処理
//============================================================
void CMultiValue::SetPriority(const int nPriority)
{
	// 引数の優先順位を設定
	CObject::SetPriority(nPriority);	// 自身
	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字の優先順位の設定
		assert(rList != nullptr);
		rList->SetPriority(nPriority);
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CMultiValue::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	向きの設定処理
//============================================================
void CMultiValue::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 設定する向きを保存
	m_rot = rRot;

	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字向きの設定
		assert(rList != nullptr);
		rList->SetVec3Rotation(rRot);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	大きさの設定処理
//============================================================
void CMultiValue::SetVec3Sizing(const D3DXVECTOR3& rSize)
{
	// 設定する大きさを保存
	m_size = rSize;

	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字大きさの設定
		assert(rList != nullptr);
		rList->SetVec3Sizing(rSize);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	生成処理
//============================================================
CMultiValue *CMultiValue::Create
(
	const CValue::EType type,	// 数字種類
	const int nNum,				// 数字
	const int nDigit,			// 桁数
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize,	// 大きさ
	const D3DXVECTOR3& rSpace,	// 空白
	const EAlignX alignX,		// 横配置
	const EAlignY alignY,		// 縦配置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol		// 色
)
{
	// マルチ数字の生成
	CMultiValue *pMultiValue = new CMultiValue;
	if (pMultiValue == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// マルチ数字の初期化
		if (FAILED(pMultiValue->Init()))
		{ // 初期化に失敗した場合

			// マルチ数字の破棄
			SAFE_DELETE(pMultiValue);
			return nullptr;
		}

		// 桁数を設定
		if (FAILED(pMultiValue->SetDigit(nDigit)))
		{ // 設定に失敗した場合

			// マルチ数字の破棄
			SAFE_DELETE(pMultiValue);
			return nullptr;
		}

		// 数字を設定
		pMultiValue->SetNum(nNum);

		// 数字種類を設定
		pMultiValue->SetType(type);

		// 位置を設定
		pMultiValue->SetVec3Position(rPos);

		// 向きを設定
		pMultiValue->SetVec3Rotation(rRot);

		// 大きさを設定
		pMultiValue->SetVec3Sizing(rSize);

		// 色を設定
		pMultiValue->SetColor(rCol);

		// 空白を設定
		pMultiValue->SetSpace(rSpace);

		// 横配置を設定
		pMultiValue->SetAlignX(alignX);

		// 縦配置を設定
		pMultiValue->SetAlignY(alignY);

		// 確保したアドレスを返す
		return pMultiValue;
	}
}

//============================================================
//	数値の加算処理
//============================================================
void CMultiValue::AddNum(const int nNum)
{
	// 数字を引数分加算
	m_nNum += nNum;

	// 数字の補正
	useful::LimitNum(m_nNum, m_nMin, m_nMax);

	// 数字のテクスチャ座標の設定
	SetTexNum();
}

//============================================================
//	数値の設定処理
//============================================================
void CMultiValue::SetNum(const int nNum)
{
	// 数字を引数の値に設定
	m_nNum = nNum;

	// 数字の補正
	useful::LimitNum(m_nNum, m_nMin, m_nMax);

	// 数字のテクスチャ座標の設定
	SetTexNum();
}

//============================================================
//	最小値の設定処理
//============================================================
void CMultiValue::SetMin(const int nMin)
{
	// 例外処理
	assert(nMin >= 0 && nMin <= m_nMax);

	// 引数の最小値を設定
	m_nMin = nMin;

	// 数字の補正
	useful::LimitNum(m_nNum, m_nMin, m_nMax);
}

//============================================================
//	最大値の設定処理
//============================================================
void CMultiValue::SetMax(const int nMax)
{
#if _DEBUG	// 最大値が正規かチェック

	int nLimit = 1;	// 最大値の計算用
	int nDigit = (int)m_listValue.size();	// 桁数
	for (int i = 0; i < nDigit; i++)
	{ // 数字の桁数分繰り返す

		// 桁数を増やす
		nLimit *= 10;
	}

	// 例外処理
	assert(nMax <= nLimit - 1 && nMax >= m_nMin);

#endif	// _DEBUG

	// 引数の最大値を設定
	m_nMax = nMax;

	// 数字の補正
	useful::LimitNum(m_nNum, m_nMin, m_nMax);
}

//============================================================
//	桁数の設定処理
//============================================================
HRESULT CMultiValue::SetDigit(const int nDigit)
{
	//--------------------------------------------------------
	//	数字オブジェクトの破棄・生成
	//--------------------------------------------------------
	// 数字オブジェクトの破棄
	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字の終了
		SAFE_UNINIT(rList);
	}

	// 数字リストをクリア
	m_listValue.clear();

	// 数字オブジェクトの生成
	for (int nCntValue = 0; nCntValue < nDigit; nCntValue++)
	{ // 桁数分繰り返す

		// 数字の生成
		CValue *pValue = CValue::Create(CValue::TYPE_NORMAL);
		if (pValue == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 自動更新・自動描画をOFFにする
		pValue->SetEnableUpdate(false);
		pValue->SetEnableDraw(false);

		// リストに数字オブジェクトを追加
		m_listValue.push_back(pValue);
	}

	//--------------------------------------------------------
	//	基本情報の再設定
	//--------------------------------------------------------
	// 向きを設定
	SetVec3Rotation(m_rot);

	// 大きさを設定
	SetVec3Sizing(m_size);

	// 色を設定
	SetColor(m_col);

	//--------------------------------------------------------
	//	制限値の設定
	//--------------------------------------------------------
	int nLimit = 1;	// 最大値の計算用
	for (int i = 0; i < nDigit; i++)
	{ // 数字の桁数分繰り返す

		// 桁数を増やす
		nLimit *= 10;
	}

	// 最小値の設定
	SetMin(0);

	// 最大値の設定
	SetMax(nLimit - 1);

	// 成功を返す
	return S_OK;
}

//============================================================
//	横配置の設定処理
//============================================================
void CMultiValue::SetAlignX(const EAlignX align)
{
	// 引数の横配置を設定
	m_alignX = align;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	縦配置の設定処理
//============================================================
void CMultiValue::SetAlignY(const EAlignY align)
{
	// 引数の縦配置を設定
	m_alignY = align;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	種類の設定処理
//============================================================
void CMultiValue::SetType(const CValue::EType type)
{
	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字種類の設定
		assert(rList != nullptr);
		rList->SetType(type);
	}
}

//============================================================
//	色の設定処理
//============================================================
void CMultiValue::SetColor(const D3DXCOLOR& rCol)
{
	// 設定する色を保存
	m_col = rCol;

	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字色の設定
		assert(rList != nullptr);
		rList->SetColor(rCol);
	}
}

//============================================================
//	空白の設定処理
//============================================================
void CMultiValue::SetSpace(const D3DXVECTOR3& rSpace)
{
	// 引数の空白を設定
	m_space = rSpace;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	数字全体の横幅取得処理
//============================================================
float CMultiValue::GetValueWidth(void) const
{
	// 数字がない場合抜ける
	if ((int)m_listValue.size() <= 0) { assert(false); return 0.0f; }

	float fValueWidth = 0.0f;	// 数字全体の縦幅
	int nEndNumID = (int)m_listValue.size() - 1;	// 終端数字のインデックス
	for (int i = 0; i < nEndNumID; i++)
	{ // 終端数字を抜いた桁数分繰り返す

		// 次の数字までの列間を加算
		fValueWidth += m_space.x;
	}

	// 先頭と終端の数字の無視されたサイズを加算
	fValueWidth += m_listValue.front()->GetVec3Sizing().x * 0.5f;	// 先頭数字の原点左サイズ
	fValueWidth += m_listValue.back()->GetVec3Sizing().x * 0.5f;	// 終端数字の原点右サイズ

	// 数字全体の縦幅を返す
	return fValueWidth;
}

//============================================================
//	数字全体の縦幅取得処理
//============================================================
float CMultiValue::GetValueHeight(void) const
{
	// 数字がない場合抜ける
	if ((int)m_listValue.size() <= 0) { assert(false); return 0.0f; }

	float fValueHeight = 0.0f;	// 数字全体の縦幅
	int nEndNumID = (int)m_listValue.size() - 1;	// 終端数字のインデックス
	for (int i = 0; i < nEndNumID; i++)
	{ // 終端数字を抜いた桁数分繰り返す

		// 次の数字までの行間を加算
		fValueHeight += m_space.y;
	}

	// 先頭と終端の数字の無視されたサイズを加算
	fValueHeight += m_listValue.front()->GetVec3Sizing().y * 0.5f;	// 先頭数字の原点上サイズ
	fValueHeight += m_listValue.back()->GetVec3Sizing().y * 0.5f;	// 終端数字の原点下サイズ

	// 数字全体の縦幅を返す
	return fValueHeight;
}

//============================================================
//	数字全体の大きさ取得処理
//============================================================
D3DXVECTOR3 CMultiValue::GetValueSize(void) const
{
	// 数字がない場合抜ける
	if ((int)m_listValue.size() <= 0) { assert(false); return VEC3_ZERO; }

	// 数字全体の大きさを返す
	return D3DXVECTOR3(GetValueWidth(), GetValueHeight(), 0.0f);
}

//============================================================
//	相対位置の設定処理
//============================================================
void CMultiValue::SetPositionRelative(void)
{
	// 数字がない場合抜ける
	if ((int)m_listValue.size() <= 0) { return; }

	D3DXVECTOR3 sizeValue	= GetValueSize() * 0.5f;	// 数字全体の大きさ
	D3DXVECTOR3 sizeHead	= m_listValue.front()->GetVec3Sizing() * 0.5f;		// 先頭数字の大きさ
	D3DXVECTOR3 rotStart	= D3DXVECTOR3(m_rot.z + HALF_PI, m_rot.z, 0.0f);	// 文字の開始向き

	D3DXVECTOR3 posOffset = VEC3_ZERO;	// 文字の開始オフセット
	posOffset.x = -sizeValue.x + sizeHead.x - (sizeValue.x * (m_alignX - 1));
	posOffset.y = -sizeValue.y + sizeHead.y - (sizeValue.y * (m_alignY - 1));

	D3DXVECTOR3 posStart = VEC3_ZERO;	// 文字の開始位置
	posStart.x = m_pos.x + sinf(rotStart.x) * posOffset.x + sinf(rotStart.y) * posOffset.y;	// 開始位置X
	posStart.y = m_pos.y + cosf(rotStart.x) * posOffset.x + cosf(rotStart.y) * posOffset.y;	// 開始位置Y

	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 位置を反映
		assert(rList != nullptr);
		rList->SetVec3Position(posStart);

		// 次の設定座標の開始点を保存
		posStart.x += sinf(rotStart.x) * m_space.x + sinf(rotStart.y) * m_space.y;
		posStart.y += cosf(rotStart.x) * m_space.x + cosf(rotStart.y) * m_space.y;
	}
}

//============================================================
//	数字のテクスチャ座標の設定処理
//============================================================
void CMultiValue::SetTexNum(void)
{
	// 数字を桁数ごとに分解
	int nDigit = (int)m_listValue.size();	// 数値の桁数
	int *pNumDivide = new int[nDigit];		// 数値の分解用配列
	useful::ZeroClear(pNumDivide);			// 配列のメモリクリア
	useful::DivideDigitNum
	( // 引数
		pNumDivide,	// 分解結果の格納配列
		m_nNum,		// 分解する数値
		nDigit - 1	// 分解する数字の桁数
	);

	// 分解した数値を反映
	int nCntDigit = 0;	// 桁数インデックス
	for (auto& rList : m_listValue)
	{ // 数字の桁数分繰り返す

		// 数字の設定
		assert(rList != nullptr);
		rList->SetNumber(pNumDivide[nCntDigit]);

		// 桁数を加算
		nCntDigit++;
	}

	// 数値の分解用配列を破棄
	SAFE_DEL_ARRAY(pNumDivide);
}
