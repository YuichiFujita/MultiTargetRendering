//============================================================
//
//	文字列2D処理 [string2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "string2D.h"
#include "manager.h"
#include "font.h"
#include "char2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 6;	// 文字列2Dの優先順位
}

//************************************************************
//	子クラス [CString2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CString2D::CString2D() : CObject(CObject::LABEL_UI, CObject::DIM_2D, PRIORITY),
	m_ppChar		(nullptr),			// 文字ポリゴンの情報
	m_pFontChar		(nullptr),			// フォント文字
	m_pos			(VEC3_ZERO),		// 位置
	m_rot			(VEC3_ZERO),		// 向き
	m_col			(XCOL_WHITE),		// 色
	m_alignX		(XALIGN_CENTER),	// 横配置
	m_fCharHeight	(0.0f)				// 文字の縦幅
{

}

//============================================================
//	デストラクタ
//============================================================
CString2D::~CString2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CString2D::Init(void)
{
	// メンバ変数を初期化
	m_ppChar		= nullptr;			// 文字ポリゴンの情報
	m_pFontChar		= nullptr;			// フォント文字
	m_pos			= VEC3_ZERO;		// 位置
	m_rot			= VEC3_ZERO;		// 向き
	m_col			= XCOL_WHITE;		// 色
	m_alignX		= XALIGN_CENTER;	// 横配置
	m_fCharHeight	= 0.0f;				// 文字の縦幅

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CString2D::Uninit(void)
{
	for (int i = 0; i < (int)m_wsStr.size(); i++)
	{ // 文字数分繰り返す

		// 文字の破棄
		SAFE_UNINIT(m_ppChar[i]);
	}

	// 文字列の破棄
	SAFE_DEL_ARRAY(m_ppChar);

	// オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CString2D::Update(const float fDeltaTime)
{
	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	描画処理
//============================================================
void CString2D::Draw(CShader * /*pShader*/)
{

}

//============================================================
//	描画状況の設定処理
//============================================================
void CString2D::SetEnableDraw(const bool bDraw)
{
	for (int i = 0; i < (int)m_wsStr.size(); i++)
	{ // 文字数分繰り返す

		// 文字の描画状況を設定
		m_ppChar[i]->SetEnableDraw(bDraw);
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CString2D::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	向きの設定処理
//============================================================
void CString2D::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 設定する向きを保存
	m_rot = rRot;

	for (int i = 0; i < (int)m_wsStr.size(); i++)
	{ // 文字数分繰り返す

		// 文字向きの設定
		assert(m_ppChar[i] != nullptr);
		m_ppChar[i]->SetVec3Rotation(rRot);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	生成処理
//============================================================
CString2D *CString2D::Create
(
	const std::string &rFilePass,	// フォントパス
	const bool bItalic,				// イタリック
	const std::wstring &rStr,		// 指定文字列
	const D3DXVECTOR3 &rPos,		// 原点位置
	const float fHeight,			// 文字縦幅
	const EAlignX alignX,			// 横配置
	const D3DXVECTOR3& rRot,		// 原点向き
	const D3DXCOLOR& rCol			// 色
)
{
	// 文字列2Dの生成
	CString2D *pString2D = new CString2D;
	if (pString2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 文字列2Dの初期化
		if (FAILED(pString2D->Init()))
		{ // 初期化に失敗した場合

			// 文字列2Dの破棄
			SAFE_DELETE(pString2D);
			return nullptr;
		}

		// フォントを設定
		pString2D->SetFont(rFilePass, bItalic);

		// 文字列を設定
		if (FAILED(pString2D->SetString(rStr)))
		{ // 設定に失敗した場合

			// 文字列2Dの破棄
			SAFE_DELETE(pString2D);
			return nullptr;
		}

		// 原点位置を設定
		pString2D->SetVec3Position(rPos);

		// 原点向きを設定
		pString2D->SetVec3Rotation(rRot);

		// 色を設定
		pString2D->SetColor(rCol);

		// 文字縦幅を設定
		pString2D->SetCharHeight(fHeight);

		// 横配置を設定
		pString2D->SetAlignX(alignX);

		// 確保したアドレスを返す
		return pString2D;
	}
}

//============================================================
//	文字列の設定処理
//============================================================
HRESULT CString2D::SetString(const std::wstring& rStr)
{
	// 文字数を保存する
	int nOldStrLen = (int)m_wsStr.size();	// 破棄する文字数
	int nCurStrLen = (int)rStr.size();		// 生成する文字数

	// 指定文字列を変更
	m_wsStr = rStr;

	//--------------------------------------------------------
	//	既に使用している文字を破棄
	//--------------------------------------------------------
	for (int i = 0; i < nOldStrLen; i++)
	{ // 破棄する文字数分繰り返す

		// 文字を破棄
		SAFE_UNINIT(m_ppChar[i]);
	}

	// 文字列を破棄
	SAFE_DEL_ARRAY(m_ppChar);

	//--------------------------------------------------------
	//	新しい文字を生成
	//--------------------------------------------------------
	// 文字を格納する配列を生成
	m_ppChar = new CChar2D * [nCurStrLen];
	for (int i = 0; i < nCurStrLen; i++)
	{ // 生成する文字数分繰り返す

		// 文字を生成
		m_ppChar[i] = CChar2D::Create
		( // 引数
			m_pFontChar->GetFilePass(),	// フォントパス
			m_pFontChar->GetItalic(),	// イタリック
			m_wsStr[i],		// 指定文字
			m_pos,			// 原点位置
			m_fCharHeight,	// 文字縦幅
			m_rot,			// 原点向き
			m_col			// 色
		);
		if (m_ppChar[i] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}
	}

	// 相対位置の設定
	SetPositionRelative();

	// 成功を返す
	return S_OK;
}

//============================================================
//	フォントの設定処理
//============================================================
void CString2D::SetFont
(
	const std::string &rFilePass,	// フォントパス
	const bool bItalic				// イタリック
)
{
	// フォント文字情報を設定
	CFont *pFont = GET_MANAGER->GetFont();	// フォント情報
	m_pFontChar = pFont->Regist(rFilePass, bItalic).pFontChar;

	// 文字列の再設定
	SetString(m_wsStr);
}

//============================================================
//	色の設定処理
//============================================================
void CString2D::SetColor(const D3DXCOLOR& rCol)
{
	// 設定する色を保存
	m_col = rCol;

	for (int i = 0; i < (int)m_wsStr.size(); i++)
	{ // 文字数分繰り返す

		// 文字色の設定
		assert(m_ppChar[i] != nullptr);
		m_ppChar[i]->SetColor(rCol);
	}
}

//============================================================
//	文字縦幅の設定処理
//============================================================
void CString2D::SetCharHeight(const float fHeight)
{
	// 設定する文字の縦幅を保存
	m_fCharHeight = fHeight;

	for (int i = 0; i < (int)m_wsStr.size(); i++)
	{ // 文字数分繰り返す

		// 文字縦幅の設定
		assert(m_ppChar[i] != nullptr);
		m_ppChar[i]->SetCharHeight(fHeight);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	横配置の設定処理
//============================================================
void CString2D::SetAlignX(const CString2D::EAlignX alignX)
{
	// 引数の横配置を設定
	m_alignX = alignX;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	文字列の横幅取得処理
//============================================================
float CString2D::GetStrWidth(void) const
{
	// 文字列がない場合抜ける
	if ((int)m_wsStr.size() <= 0) { assert(false); return 0.0f; }

	float fStrWidth = 0.0f;	// 文字列の横幅
	int nEndCharID = (int)m_wsStr.size() - 1;	// 終端文字のインデックス

	for (int i = 0; i < nEndCharID; i++)
	{ // 終端文字を抜いた文字数分繰り返す

		// 次の文字までの距離を加算
		assert(m_ppChar[i] != nullptr);
		fStrWidth += m_ppChar[i]->GetNext();
	}

	// 先頭文字のサイズをブラックボックス左からの大きさに補正
	assert(m_ppChar[0] != nullptr);
	float fHeadWidth = m_ppChar[0]->GetVec3Sizing().x * 0.5f;		// 先頭文字の横幅
	fStrWidth += fHeadWidth - m_ppChar[0]->GetOffsetOrigin();		// 原点より前の空間を加算
	fStrWidth -= fHeadWidth + m_ppChar[0]->GetOffsetBlackBoxLU().x;	// ブラックボックス開始より前の空間を減算

	// 終端文字のサイズをブラックボックス右までの大きさに補正
	assert(m_ppChar[nEndCharID] != nullptr);
	float fTailWidth = m_ppChar[nEndCharID]->GetVec3Sizing().x * 0.5f;			// 終端文字の横幅
	fStrWidth -= fTailWidth - m_ppChar[nEndCharID]->GetOffsetOrigin();			// 原点より前の空間を減算
	fStrWidth += fTailWidth + m_ppChar[nEndCharID]->GetOffsetBlackBoxRD().x;	// ブラックボックス終了より前の空間を加算

	// 文字列の横幅を返す
	return fStrWidth;
}

//============================================================
//	文字の取得処理
//============================================================
CChar2D *CString2D::GetChar2D(const int nCharID) const
{
	// 文字列がない場合抜ける
	int nStrLen = (int)m_wsStr.size();
	if (nStrLen <= 0) { assert(false); return nullptr; }

	// インデックスが範囲外の場合抜ける
	if (nCharID <= NONE_IDX || nCharID >= nStrLen) { assert(false); return nullptr; }

	// 引数インデックスの文字を返す
	return m_ppChar[nCharID];
}

//============================================================
//	相対位置の設定処理
//============================================================
void CString2D::SetPositionRelative(void)
{
	// 文字列がない場合抜ける
	if ((int)m_wsStr.size() <= 0) { return; }

	assert(m_ppChar[0] != nullptr);
	float fHeadOffsetOrigin	= m_ppChar[0]->GetOffsetOrigin();			// 先頭文字の原点オフセット
	float fHeadOffsetLU	= m_ppChar[0]->GetOffsetBlackBoxLU().x;			// 先頭文字のブラックボックスオフセット
	float fHeadRot		= m_ppChar[0]->GetVec3Rotation().z - HALF_PI;	// 先頭文字の向き
	float fHeadWidth	= m_ppChar[0]->GetVec3Sizing().x * 0.5f;		// 先頭文字の横幅
	float fStrWidth		= GetStrWidth() * 0.5f;							// 文字列全体の横幅

	float fStartOffset = fStrWidth;	// 文字の開始位置オフセット
	fStartOffset -= fHeadWidth - fHeadOffsetOrigin;	// 先頭文字の原点分ずらす
	fStartOffset += fHeadWidth + fHeadOffsetLU;		// 先頭文字のブラックボックス分ずらす
	fStartOffset += (fStrWidth * (m_alignX - 1));	// 指定された横配置にする

	D3DXVECTOR3 posStart = VEC3_ZERO;	// 文字の開始位置
	posStart.x = m_pos.x + sinf(fHeadRot) * fStartOffset;	// 開始位置X
	posStart.y = m_pos.y + cosf(fHeadRot) * fStartOffset;	// 開始位置Y

	for (int i = 0; i < (int)m_wsStr.size(); i++)
	{ // 文字数分繰り返す

		assert(m_ppChar[i] != nullptr);
		float fOffset = m_ppChar[i]->GetOffsetOrigin();	// 原点オフセット
		float fNext = m_ppChar[i]->GetNext();			// 次文字までの距離

		// 設定座標に原点オフセットを与える
		posStart.x -= sinf(fHeadRot) * fOffset;
		posStart.y -= cosf(fHeadRot) * fOffset;

		// 位置を反映
		m_ppChar[i]->SetVec3Position(posStart);

		// 次の設定座標の開始点を保存
		float fNextStart = fOffset - fNext;	// 次の開始点
		posStart.x += sinf(fHeadRot) * fNextStart;
		posStart.y += cosf(fHeadRot) * fNextStart;
	}
}
