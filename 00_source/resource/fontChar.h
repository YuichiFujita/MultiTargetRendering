//============================================================
//
//	フォント文字ヘッダー [fontChar.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _FONT_CHAR_H_
#define _FONT_CHAR_H_

//************************************************************
//	クラス定義
//************************************************************
// フォント文字クラス
class CFontChar
{
public:
	// コンストラクタ
	CFontChar(const HFONT hFont, const std::string &rFilePass, const bool bItalic);

	// デストラクタ
	~CFontChar();

	// 矩形の座標情報
	struct SRectangle
	{
		POSGRID2 lu;	// 左上座標
		POSGRID2 rd;	// 右下座標
	};

	// フォント文字構造体
	struct SChar
	{
		// コンストラクタ
		SChar() { memset(this, 0, sizeof(*this)); }

		// メンバ変数
		GLYPHMETRICS glyph;			// フォントグリフの情報
		OUTLINETEXTMETRIC outline;	// フォントアウトラインの情報
		TEXTMETRIC text;			// フォントテキストの情報
		SRectangle offsetBlackBox;	// ブラックボックスの中心からのオフセット
		int nTexID;					// テクスチャインデックス
	};

	// メンバ関数
	HRESULT Init(void);	// フォント文字初期化
	void Uninit(void);	// フォント文字終了
	SChar Regist(const wchar_t wcChar);	// フォント文字登録
	const HFONT GetFont(void)			{ return m_hFont; }		// フォントへのポインタ
	const std::string GetFilePass(void)	{ return m_sFilePass; }	// フォントパス
	const bool GetItalic(void)			{ return m_bItalic; }	// イタリック

	// 静的メンバ関数
	static CFontChar *Create(const HFONT hFont, const std::string &rFilePass, const bool bItalic);	// 生成
	static void Release(CFontChar *&pFontChar);	// 破棄

private:
	// メンバ関数
	BYTE *CreateBitMap(SChar *pChar, HDC hDC, const wchar_t wcChar);	// ビットマップ生成・文字情報保存
	HRESULT CreateTexture(SChar *pChar, BYTE *pBitMap);					// テクスチャ生成・作成

	// メンバ変数
	std::map<wchar_t, SChar> m_mapChar;	// フォント文字連想配列
	const HFONT m_hFont;				// フォントへのポインタ
	const std::string m_sFilePass;		// フォントパス
	const bool m_bItalic;				// イタリック
};

#endif	// _FONT_CHAR_H_
