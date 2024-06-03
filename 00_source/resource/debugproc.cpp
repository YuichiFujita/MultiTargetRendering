//==========================================================
//
// デバッグ表示処理 [debugproc.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "main.h"
#include "debugproc.h"
#include <string.h>
#include <stdio.h>
#include "manager.h"
#include "renderer.h"

//**********************************************************
//	定数宣言
//**********************************************************
namespace
{
	const int MAX_FLOATNUM = 2;	// 小数点以下の表示桁数
}

//**********************************************************
//	静的メンバ変数宣言
//**********************************************************
LPD3DXFONT CDebugProc::m_pFont = nullptr;	// デバッグフォントへのポインタ

//==========================================================
// コンストラクタ
//==========================================================
CDebugProc::CDebugProc()
{
	//デバッグ表示情報のクリア
	m_bDisp = false;
	m_pFont = nullptr;
	memset(&m_aStr[0][0], 0, sizeof(m_aStr));
}

//==========================================================
// デストラクタ
//==========================================================
CDebugProc::~CDebugProc()
{
	
}

//==========================================================
//デバッグ表示の初期化処理
//==========================================================
HRESULT CDebugProc::Init(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスへのポインタ

	// デバッグ表示情報の初期化
	m_bDisp = false;
	m_pFont = nullptr;
	memset(&m_aStr[0][0], 0, sizeof(m_aStr));

	// デバッグ表示用フォントの生成
	if (FAILED(D3DXCreateFont(pDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &m_pFont)))
	{ // フォントの生成に失敗した場合

		return E_FAIL;
	}

#ifdef _DEBUG

	// 初期表示設定
	m_bDisp = true;

#else

	// 初期表示設定
	m_bDisp = false;

#endif

	// 成功を返す
	return S_OK;
}

//==========================================================
//デバッグ表示の終了処理
//==========================================================
void CDebugProc::Uninit(void)
{
	// デバッグ表示用フォントの破棄
	SAFE_RELEASE(m_pFont);
}

//==========================================================
//デバッグ表示の更新処理
//==========================================================
void CDebugProc::Update(void)
{

}

//==========================================================
//デバッグ表示の描画処理
//==========================================================
void CDebugProc::Draw(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	if (m_bDisp == true)
	{//デバックモードがオンの時

		//テキストの描画
		m_pFont->DrawText(nullptr, &m_aStr[POINT_CENTER][0], NONE_IDX, &rect, DT_CENTER, D3DCOLOR_RGBA(0, 0, 0, 255));
		m_pFont->DrawText(nullptr, &m_aStr[POINT_LEFT][0],   NONE_IDX, &rect, DT_LEFT,   D3DCOLOR_RGBA(0, 0, 0, 255));
		m_pFont->DrawText(nullptr, &m_aStr[POINT_RIGHT][0],  NONE_IDX, &rect, DT_RIGHT,  D3DCOLOR_RGBA(0, 0, 0, 255));
	}

	//デバッグ表示情報のクリア
	memset(&m_aStr[0][0], 0, sizeof(m_aStr));
}

//==========================================================
//デバッグ表示の設定処理
//==========================================================
void CDebugProc::Print(const EPoint point, const char *fmt, ...)
{
	va_list args;
	char aString[debugproc::MAX_DEBUGSTRING];		// 指定文字格納用
	char aSaveString[debugproc::MAX_DEBUGSTRING];	// 可変引数中身格納用
	int nLength = 0;	// 可変引数内の文字の長さ
	int nStopLength;	// 可変引数挿入場所より

	//文字列の代入
	strcpy(&aString[0], fmt);

	va_start(args, fmt);

	for (char *p = &aString[0]; *p != '\0'; p++)
	{
		if (*p == '%')
		{//確認文字がある場合

			p++;	//確認文字まで進める

			nLength = 0;	// 文字の長さリセット

			// 指定された種類ごとに変数の値を抜き出す(va_arg)
			switch (*p)
			{
			case 'd':	//数字

				sprintf(&aSaveString[0], "%d", va_arg(args, int));

				break;
			case 'f':	//小数

				sprintf(&aSaveString[0], "%f", va_arg(args, double));

				//文字列の長さを取得
				nLength = (int)strlen(&aSaveString[0]);

				//小数点以下の1文字目まで確認
				for (int nCntlength = 0; aSaveString[nCntlength] != '\0'; nCntlength++)
				{
					if (aSaveString[nCntlength] == '.')
					{//小数点があった場合

						//小数点以下の桁数を求める
						int nMin = (int)strlen(&aSaveString[nCntlength + 1]);

						//小数点より先の文字数と指定した値までの差分を求める
						nMin -= MAX_FLOATNUM;

						//文字列の長さを小数点以下第二位までに補正
						nLength -= nMin;
					}
				}

				break;
			case 'c':	//文字

				sprintf(&aSaveString[0], "%c", va_arg(args, char));

				break;
			case 's':	//文字列

				sprintf(&aSaveString[0], "%s", va_arg(args, const char*));

				break;
			}

			if (nLength == 0)
			{// 文字の長さを取得していない場合

				//文字列の長さを取得
				nLength = (int)strlen(&aSaveString[0]);
			}

			nStopLength = (int)strlen(p) + 1;

			//メモリ内の確認文字(%～)より後ろの文字列をずらす
			memmove(p + nLength - 2, p, nStopLength);

			p--;	//ポインタを%まで戻す

			//可変引数を指定された場所に挿入
			memcpy(p, &aSaveString[0], nLength);
		}
	}

	va_end(args);

	//文字列を連結する
	strcat(&m_aStr[point][0], &aString[0]);
}

//==========================================================
//	生成処理
//==========================================================
CDebugProc *CDebugProc::Create(void)
{
	// デバッグ表示の生成
	CDebugProc *pDebugProc = new CDebugProc;
	if (pDebugProc == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// デバッグ表示の初期化
		if (FAILED(pDebugProc->Init()))
		{ // 初期化に失敗した場合

			// デバッグ表示の破棄
			SAFE_DELETE(pDebugProc);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pDebugProc;
	}
}

//==========================================================
//	破棄処理
//==========================================================
void CDebugProc::Release(CDebugProc *&prDebugProc)
{
	// デバッグプロックの終了
	assert(prDebugProc != nullptr);
	prDebugProc->Uninit();

	// メモリ開放
	SAFE_DELETE(prDebugProc);
}
