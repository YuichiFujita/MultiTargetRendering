//============================================================
//
//	文字送りテキスト2D処理 [scrollText2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "scrollText2D.h"
#include "string2D.h"
#include "char2D.h"

//************************************************************
//	子クラス [CScrollText2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CScrollText2D::CScrollText2D() :
	m_nNextID	(0),	// 次表示する文字インデックス
	m_fNextTime	(0.0f),	// 次表示するまでの時間
	m_fCurTime	(0.0f),	// 現在の待機時間
	m_bScroll	(false)	// 文字送り状況
{
	// 全文字情報配列をクリア
	m_vecChar.clear();
}

//============================================================
//	デストラクタ
//============================================================
CScrollText2D::~CScrollText2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CScrollText2D::Init(void)
{
	// メンバ変数を初期化
	m_nNextID	= 0;		// 次表示する文字インデックス
	m_fNextTime	= 0.0f;		// 次表示するまでの時間
	m_fCurTime	= 0.0f;		// 現在の待機時間
	m_bScroll	= false;	// 文字送り状況

	// 全文字情報配列を初期化
	m_vecChar.clear();

	// テキスト2Dの初期化
	if (FAILED(CText2D::Init()))
	{ // 初期化に失敗した場合

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
void CScrollText2D::Uninit(void)
{
	// 全文字情報配列をクリア
	m_vecChar.clear();

	// テキスト2Dの終了
	CText2D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CScrollText2D::Update(const float fDeltaTime)
{
	// 文字送りの更新
	UpdateScroll(fDeltaTime);

	// テキスト2Dの更新
	CText2D::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CScrollText2D::Draw(CShader *pShader)
{
	// テキスト2Dの描画
	CText2D::Draw(pShader);
}

//============================================================
//	描画状況の設定処理
//============================================================
void CScrollText2D::SetEnableDraw(const bool bDraw)
{
	// 次表示する文字インデックスを描画設定に応じて反映
	m_nNextID = (bDraw) ? (int)m_vecChar.size() - 1 : 0;	// ONなら最後尾、OFFなら先頭

	// 現在の待機時間を初期化
	m_fCurTime = 0.0f;

	// 描画状況の設定
	CText2D::SetEnableDraw(bDraw);
}

//============================================================
//	文字列の追加処理
//============================================================
HRESULT CScrollText2D::AddString(const std::wstring& rStr)
{
	// 文字列の追加
	if (FAILED(CText2D::AddString(rStr)))
	{ // 追加に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	int nTailStrID = GetNumString() - 1;			// 最後尾の文字列インデックス
	CString2D *pTailStr = GetString2D(nTailStrID);	// 最後尾の文字列情報

	// 最後尾文字列の自動描画をOFFにする
	pTailStr->SetEnableDraw(false);

	// 文字列内の文字を配列に追加
	int nNumChar = pTailStr->GetNumChar();	// 文字数
	for (int nCntChar = 0; nCntChar < nNumChar; nCntChar++)
	{ // 文字数分繰り返す

		// 最後尾に文字アドレスを追加
		m_vecChar.push_back(pTailStr->GetChar2D(nCntChar));
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	文字列の削除処理
//============================================================
void CScrollText2D::DeleteString(const int nStrID)
{
	int nDelHead = 0;	// 削除する先頭文字インデックス
	for (int i = 0; i < nStrID; i++)
	{ // 削除する文字列の手前まで繰り返す

		// 削除する文字の先頭インデックスまで値を進める
		nDelHead += GetString2D(i)->GetNumChar();
	}

	// 削除するイテレーターの先頭を求める
	auto itrDelHead = m_vecChar.begin();	// 削除する先頭イテレーター
	std::advance(itrDelHead, nDelHead);		// 先頭まで進める

	// 削除するイテレーターの最後尾を求める
	auto itrDelTail = m_vecChar.begin();	// 削除する最後尾イテレーター
	std::advance(itrDelTail, nDelHead + GetString2D(nStrID)->GetNumChar());	// 最後尾まで進める

	// 先頭から最後尾までのイテレーターを削除
	m_vecChar.erase(itrDelHead, itrDelTail);

	// 文字列の削除
	CText2D::DeleteString(nStrID);

	// 文字インデックスを制限する
	useful::LimitMaxNum(m_nNextID, (int)m_vecChar.size());
}

//============================================================
//	文字列の全削除処理
//============================================================
void CScrollText2D::DeleteStringAll(void)
{
	// 文字列の全削除
	CText2D::DeleteStringAll();

	// 全文字情報配列をクリア
	m_vecChar.clear();

	// 文字インデックスを初期化
	m_nNextID = 0;
}

//============================================================
//	生成処理
//============================================================
CScrollText2D *CScrollText2D::Create
(
	const std::string &rFilePass,		// フォントパス
	const bool bItalic,					// イタリック
	const D3DXVECTOR3& rPos,			// 原点位置
	const float fNextTime,				// 文字表示の待機時間
	const float fCharHeight,			// 文字縦幅
	const float fLineHeight,			// 行間縦幅
	const CString2D::EAlignX alignX,	// 横配置
	const EAlignY alignY,				// 縦配置
	const D3DXVECTOR3& rRot,			// 原点向き
	const D3DXCOLOR& rCol				// 色
)
{
	// 文字送りテキスト2Dの生成
	CScrollText2D *pScrollText2D = new CScrollText2D;
	if (pScrollText2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 文字送りテキスト2Dの初期化
		if (FAILED(pScrollText2D->Init()))
		{ // 初期化に失敗した場合

			// 文字送りテキスト2Dの破棄
			SAFE_DELETE(pScrollText2D);
			return nullptr;
		}

		// フォントを設定
		pScrollText2D->SetFont(rFilePass, bItalic);

		// 原点位置を設定
		pScrollText2D->SetVec3Position(rPos);

		// 原点向きを設定
		pScrollText2D->SetVec3Rotation(rRot);

		// 色を設定
		pScrollText2D->SetColor(rCol);

		// 文字表示の待機時間を設定
		pScrollText2D->SetNextTime(fNextTime);

		// 文字縦幅を設定
		pScrollText2D->SetCharHeight(fCharHeight);

		// 行間縦幅を設定
		pScrollText2D->SetLineHeight(fLineHeight);

		// 横配置を設定
		pScrollText2D->SetAlignX(alignX);

		// 縦配置を設定
		pScrollText2D->SetAlignY(alignY);

		// 確保したアドレスを返す
		return pScrollText2D;
	}
}

//============================================================
//	文字送りの更新処理
//============================================================
void CScrollText2D::UpdateScroll(const float fDeltaTime)
{
	// 文字送りがOFFなら抜ける
	if (!m_bScroll) { return; }

	// 文字がない場合抜ける
	if (m_vecChar.empty()) { m_bScroll = false; return; }

	// 現在の待機時間を加算
	m_fCurTime += fDeltaTime;
	while (m_fCurTime >= m_fNextTime)
	{ // 待機し終わった場合

		// 文字の自動描画をONにする
		assert(m_vecChar[m_nNextID] != nullptr);
		m_vecChar[m_nNextID]->SetEnableDraw(true);

		// 現在の待機時間から待機時間を減算
		m_fCurTime -= m_fNextTime;

		// 次の文字インデックスに移行
		m_nNextID++;

		if (useful::LimitMaxNum(m_nNextID, (int)m_vecChar.size() - 1))
		{ // 最後の文字に到達した場合

			// 現在の待機時間を初期化
			m_fCurTime = 0.0f;

			// 文字送りをOFFにする
			m_bScroll = false;

			break;
		}
	}
}
