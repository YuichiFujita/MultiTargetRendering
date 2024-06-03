//============================================================
//
//	フォント文字処理 [fontChar.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "fontChar.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	マクロ定義
//************************************************************
#define SAFE_DEL_DC(p)	if ((p) != nullptr) { ReleaseDC(nullptr, (p)); (p) = nullptr; }	// DeleteDC関数の破棄マクロ

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const MAT2 INIT_MATRIX = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };	// 初期化マトリックス

	const CFontChar::SChar ZERO_CHAR;				// フォント文字初期値
	const UINT FORMAT_BITMAP = GGO_GRAY4_BITMAP;	// ビットマップのフォーマット
	const int MAX_GRAD = 16;						// 上のビットマップの最大階調
	const int MIN_BLACKBOX = 4;						// ブラックボックスの横幅の切り捨て値
}

//************************************************************
//	親クラス [CFontChar] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CFontChar::CFontChar(const HFONT hFont, const std::string &rFilePass, const bool bItalic) :
	m_hFont		(hFont),		// フォントへのポインタ
	m_sFilePass	(rFilePass),	// フォントパス
	m_bItalic	(bItalic)		// イタリック
{
	// フォント文字連想配列をクリア
	m_mapChar.clear();
}

//============================================================
//	デストラクタ
//============================================================
CFontChar::~CFontChar()
{

}

//============================================================
//	フォント文字初期化処理
//============================================================
HRESULT CFontChar::Init(void)
{
	// フォント文字連想配列を初期化
	m_mapChar.clear();

	// 成功を返す
	return S_OK;
}

//============================================================
//	フォント文字終了処理
//============================================================
void CFontChar::Uninit(void)
{
	// フォント文字連想配列をクリア
	m_mapChar.clear();
}

//============================================================
//	フォント文字登録処理 (名前)
//============================================================
CFontChar::SChar CFontChar::Regist(const wchar_t wcChar)
{
	// 既に生成済みかを検索
	auto itr = m_mapChar.find(wcChar);	// 引数の文字を検索
	if (itr != m_mapChar.end())
	{ // 生成済みの場合

		// 読込済みのフォント文字情報を返す
		return itr->second;
	}

	// 設定用フォント文字情報を宣言
	SChar tempChar;

	// デバイスコンテキストを取得
	HDC hDC = GetDC(nullptr);
	if (hDC == nullptr)
	{ // 取得に失敗した場合

		// 初期値を返す
		assert(false);
		return ZERO_CHAR;
	}

	// 使用するフォントを選択し、過去のフォントを保存
	HFONT hOldFont = (HFONT)SelectObject(hDC, m_hFont);

	// ビットマップの生成・文字情報の保存
	BYTE *pBitMap = CreateBitMap(&tempChar, hDC, wcChar);
	if (pBitMap == nullptr)
	{ // 生成に失敗した場合

		// 初期値を返す
		assert(false);
		return ZERO_CHAR;
	}

	// テクスチャの生成・設定
	if (FAILED(CreateTexture(&tempChar, pBitMap)))
	{ // 生成に失敗した場合

		// 初期値を返す
		assert(false);
		return ZERO_CHAR;
	}

	// 保存したフォントに戻す
	SelectObject(hDC, hOldFont);

	// ビットマップを破棄
	SAFE_DEL_ARRAY(pBitMap);

	// デバイスコンテキストを破棄
	SAFE_DEL_DC(hDC);

	// フォント文字情報を保存
	m_mapChar.insert(std::make_pair(wcChar, tempChar));

	// 生成したフォント文字情報を返す
	return tempChar;
}

//============================================================
//	生成処理
//============================================================
CFontChar *CFontChar::Create(const HFONT hFont, const std::string &rFilePass, const bool bItalic)
{
	// フォント文字の生成
	CFontChar *pFontChar = new CFontChar(hFont, rFilePass, bItalic);
	if (pFontChar == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// フォント文字の初期化
		if (FAILED(pFontChar->Init()))
		{ // フォント文字初期化に失敗した場合

			// フォント文字の破棄
			SAFE_DELETE(pFontChar);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pFontChar;
	}
}

//============================================================
//	破棄処理
//============================================================
void CFontChar::Release(CFontChar *&prFontChar)
{
	// フォント文字の終了
	assert(prFontChar != nullptr);
	prFontChar->Uninit();

	// メモリ開放
	SAFE_DELETE(prFontChar);
}

//============================================================
//	ビットマップの生成・文字情報の保存
//============================================================
BYTE *CFontChar::CreateBitMap(SChar *pChar, HDC hDC, const wchar_t wcChar)
{
	// ビットマップのサイズを取得
	DWORD dwGlyphSize = GetGlyphOutlineW(hDC, wcChar, FORMAT_BITMAP, &pChar->glyph, 0, nullptr, &INIT_MATRIX);
	if (dwGlyphSize == GDI_ERROR)
	{ // 取得に失敗した場合

		// 失敗を返す
		assert(false);
		return nullptr;
	}

	// ビットマップのサイズ分メモリ確保
	BYTE *pBitMap = new BYTE[dwGlyphSize];
	if (pBitMap == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return nullptr;
	}

	// ビットマップ内の情報を取得
	dwGlyphSize = GetGlyphOutlineW(hDC, wcChar, FORMAT_BITMAP, &pChar->glyph, dwGlyphSize, pBitMap, &INIT_MATRIX);
	if (dwGlyphSize == GDI_ERROR)
	{ // 取得に失敗した場合

		// 失敗を返す
		assert(false);
		return nullptr;
	}

	// フォントアウトラインの情報を取得
	DWORD dwOutLineSize = GetOutlineTextMetrics(hDC, sizeof(OUTLINETEXTMETRIC), &pChar->outline);
	if (dwOutLineSize == GDI_ERROR)
	{ // 取得に失敗した場合

		// 失敗を返す
		assert(false);
		return nullptr;
	}

	// フォントテキストの情報を取得
	if (!GetTextMetrics(hDC, &pChar->text))
	{ // 取得に失敗した場合

		// 失敗を返す
		assert(false);
		return nullptr;
	}

	// 確保したビットマップを返す
	return pBitMap;
}

//============================================================
//	テクスチャの生成・作成
//============================================================
HRESULT CFontChar::CreateTexture(SChar *pChar, BYTE *pBitMap)
{
	int nAbsOffset = std::abs(pChar->glyph.gmptGlyphOrigin.x);	// 原点オフセットの絶対値
	int nPlusSize = nAbsOffset * 4;	// テクスチャ横幅の大きさ加算量

	POSGRID2 sizeAlignBlackBox;	// アライメントを考慮したブラックボックスの大きさ
	sizeAlignBlackBox.x = (pChar->glyph.gmBlackBoxX + 3) / 4 * 4;	// ブラックボックス横幅
	sizeAlignBlackBox.y = pChar->glyph.gmBlackBoxY;					// ブラックボックス縦幅

	POSGRID2 offsetOrigin;	// 原点のオフセット
	offsetOrigin.x = pChar->glyph.gmptGlyphOrigin.x + nAbsOffset + 1;			// 原点オフセットX
	offsetOrigin.y = pChar->text.tmAscent - pChar->glyph.gmptGlyphOrigin.y + 1;	// 原点オフセットY

	POSGRID2 sizeTexture;	// テクスチャの大きさ
	sizeTexture.x = sizeAlignBlackBox.x + nPlusSize + 2;	// テクスチャ横幅
	sizeTexture.y = (int)pChar->text.tmHeight + 2;			// テクスチャ縦幅

	// 空のテクスチャを生成・テクスチャインデックスを保存
	CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャ情報
	pChar->nTexID = pTexture->Regist(CTexture::SInfo
	( // 引数
		sizeTexture.x,		// テクスチャ横幅
		sizeTexture.y,		// テクスチャ縦幅
		1,					// ミップマップレベル
		0,					// 性質・確保オプション
		D3DFMT_A8R8G8B8,	// ピクセルフォーマット
		D3DPOOL_MANAGED		// 格納メモリ
	));

	// アライメントを考慮していないブラックボックスの大きさを設定
	POSGRID2 sizeBlackBox = POSGRID2(pChar->glyph.gmBlackBoxX, pChar->glyph.gmBlackBoxY);

	// ブラックボックスの中心からのオフセットを保存
	POSGRID2 centerTexture = sizeTexture / 2;	// テクスチャの中心座標
	pChar->offsetBlackBox.lu = offsetOrigin - centerTexture;				// 左上オフセット
	pChar->offsetBlackBox.rd = offsetOrigin + sizeBlackBox - centerTexture;	// 右下オフセット

	// ブラックボックスが小さすぎる場合書き込みを行わない
	if (sizeAlignBlackBox.x <= MIN_BLACKBOX) { return S_OK; }

	// 生成したテクスチャのポインタを取得
	LPDIRECT3DTEXTURE9 pTexChar = pTexture->GetPtr(pChar->nTexID);

	// テクスチャをロックし、ピクセル情報を取得
	D3DLOCKED_RECT lockRect;
	pTexChar->LockRect(0, &lockRect, nullptr, 0);

	// テクスチャにフォントの見た目を書き込み
	for (int nCntHeight = offsetOrigin.y; nCntHeight < offsetOrigin.y + sizeAlignBlackBox.y; nCntHeight++)
	{ // フォント原点からブラックボックス縦幅分繰り返す

		for (int nCntWidth = offsetOrigin.x; nCntWidth < offsetOrigin.x + sizeAlignBlackBox.x; nCntWidth++)
		{ // フォント原点からブラックボックス横幅分繰り返す

			// 現在のビットマップインデックスを計算
			int nBitID = nCntWidth - offsetOrigin.x + sizeAlignBlackBox.x * (nCntHeight - offsetOrigin.y);

			// ビットマップからα値を計算
			DWORD dwAlpha = pBitMap[nBitID] * 255 / MAX_GRAD;

			// テクスチャの色を計算
			DWORD dwColor = (dwAlpha << 24) | 0x00ffffff;

			// テクスチャのビットに色を書き込み
			memcpy((BYTE*)lockRect.pBits + (lockRect.Pitch * nCntHeight) + (sizeof(dwColor) * nCntWidth), &dwColor, sizeof(dwColor));
		}
	}

	// テクスチャをアンロックする
	pTexChar->UnlockRect(0);

	// 成功を返す
	return S_OK;
}
