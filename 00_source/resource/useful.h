//============================================================
//
//	便利関数ヘッダー [useful.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _USEFUL_H_
#define _USEFUL_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "renderState.h"

//************************************************************
//	マクロ定義
//************************************************************
// 便利マクロ
#define SCREEN_WIDTH	(1280)	// ウインドウの横幅
#define SCREEN_HEIGHT	(720)	// ウインドウの縦幅
#define MAX_STRING		(128)	// 文字列の最大文字数
#define MAX_FILENAME	(256)	// ファイル名の最大文字数
#define NONE_IDX		(-1)	// インデックス非使用
#define NONE_STRING		("\0")	// 文字列非使用

#define HALF_PI	(D3DX_PI * 0.5f)	// 二分の一の円周率 (π/２)
#define QRTR_PI	(D3DX_PI * 0.25f)	// 四分の一の円周率 (π/４)

#define NUM_ARRAY(a)	(sizeof((a)) / sizeof((a)[0]))	// 配列の要素数計算

// メモリ開放マクロ
#define SAFE_UNINIT(p)		if ((p) != nullptr) { (p)->Uninit();		(p) = nullptr; }	// Uninit関数の破棄マクロ
#define SAFE_FREE(p)		if ((p) != nullptr) { free((p));			(p) = nullptr; }	// free関数の破棄マクロ
#define SAFE_RELEASE(p)		if ((p) != nullptr) { (p)->Release();		(p) = nullptr; }	// Release関数の破棄マクロ
#define SAFE_DEL_OBJECT(p)	if ((p) != nullptr) { DeleteObject((p));	(p) = nullptr; }	// DeleteObject関数の破棄マクロ
#define SAFE_REF_RELEASE(p)	if ((p) != nullptr) { (p)->Release((p)); }						// 参照ポインタ付きRelease関数の破棄マクロ
#define SAFE_DELETE(p)		if ((p) != nullptr) { delete	(p);		(p) = nullptr; }	// 配列を使用しないdeleteを使用する破棄マクロ
#define SAFE_DEL_ARRAY(p)	if ((p) != nullptr) { delete[]	(p);		(p) = nullptr; }	// 配列を使用したdelete[]を使用する破棄マクロ

// マネージャー関係
#define GET_MANAGER		(CManager::GetInstance())					// マネージャーインスタンス取得
#define GET_INPUTKEY	(CManager::GetInstance()->GetKeyboard())	// キーボード情報取得
#define GET_INPUTMOUSE	(CManager::GetInstance()->GetMouse())		// マウス情報取得
#define GET_INPUTPAD	(CManager::GetInstance()->GetPad())			// パッド情報取得
#define GET_RETENTION	(CManager::GetInstance()->GetRetention())	// データ保存情報取得
#define GET_RENDERER	(CManager::GetInstance()->GetRenderer())	// レンダラー情報取得
#define GET_DEVICE		(CManager::GetInstance()->GetRenderer()->GetDevice())	// デバイス情報取得

#define GET_EFFECT			(CEffekseer::GetInstance())	// エフェクシアインスタンス取得
#define PLAY_SOUND(label)	(CManager::GetInstance()->GetSound()->Play((CSound::ELabel)(label)))	// サウンド再生

// D3DXVECTOR2関係
#define VEC2_ZERO	(D3DXVECTOR2(0.0f, 0.0f))	// 0クリア
#define VEC2_ONE	(D3DXVECTOR2(1.0f, 1.0f))	// 1クリア
#define VEC2_ALL(f)	(D3DXVECTOR2((f), (f))		// 同値クリア

// D3DXVECTOR3関係
#define VEC3_ZERO	(D3DXVECTOR3(0.0f, 0.0f, 0.0f))	// 0クリア
#define VEC3_ONE	(D3DXVECTOR3(1.0f, 1.0f, 1.0f))	// 1クリア
#define VEC3_ALL(f)	(D3DXVECTOR3((f), (f), (f)))	// 同値クリア

#define SCREEN_CENT	(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f))	// ウインドウの中央座標
#define SCREEN_SIZE	(D3DXVECTOR3((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f))	// ウインドウの画面サイズ

// POSGRID2関係
#define GRID2_ZERO	(POSGRID2(0, 0))	// 0クリア
#define GRID2_ONE	(POSGRID2(1, 1))	// 1クリア

// POSGRID3関係
#define GRID3_ZERO	(POSGRID3(0, 0, 0))	// 0クリア
#define GRID3_ONE	(POSGRID3(1, 1, 1))	// 1クリア

// D3DXCOLOR関係
#define XCOL_ABLACK		(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f))	// 黒色(透明)
#define XCOL_BLACK		(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f))	// 黒色(不透明)
#define XCOL_AWHITE		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f))	// 白色(透明)
#define XCOL_WHITE		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))	// 白色(不透明)
#define XCOL_ARED		(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f))	// 赤色(透明)
#define XCOL_RED		(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f))	// 赤色(不透明)
#define XCOL_AGREEN		(D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.0f))	// 緑色(透明)
#define XCOL_GREEN		(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f))	// 緑色(不透明)
#define XCOL_ABLUE		(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.0f))	// 青色(透明)
#define XCOL_BLUE		(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f))	// 青色(不透明)
#define XCOL_AYELLOW	(D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f))	// 黄色(透明)
#define XCOL_YELLOW		(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f))	// 黄色(不透明)
#define XCOL_ACYAN		(D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.0f))	// 水色(透明)
#define XCOL_CYAN		(D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f))	// 水色(不透明)

// RenderState関係
#define RS_BL_NORMAL	(CRenderState::SBlendAlpha(D3DBLENDOP_ADD,			D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA))	// 通常αブレンド
#define RS_BL_ADD		(CRenderState::SBlendAlpha(D3DBLENDOP_ADD,			D3DBLEND_SRCALPHA,	D3DBLEND_ONE))			// 加算αブレンド
#define RS_BL_SUB		(CRenderState::SBlendAlpha(D3DBLENDOP_REVSUBTRACT,	D3DBLEND_SRCALPHA,	D3DBLEND_ONE))			// 減算αブレンド

//************************************************************
//	構造体定義
//************************************************************
// 2整数座標
struct POSGRID2
{
	// コンストラクタ
	POSGRID2() {}
	POSGRID2(const int nx, const int ny) { x = nx; y = ny; }	// 代入

	// デストラクタ
	~POSGRID2() {}

	// 演算子オーバーロード
	bool operator==(const POSGRID2& grid) const;
	bool operator!=(const POSGRID2& grid) const;
	POSGRID2 operator+(const POSGRID2& grid) const;
	POSGRID2 operator-(const POSGRID2& grid) const;
	POSGRID2 operator*(const int nVal) const;
	POSGRID2 operator/(const int nVal) const;

	// メンバ変数
	int x;	// 方眼座標 (x)
	int y;	// 方眼座標 (y)
};

// 3整数座標
struct POSGRID3
{
	// コンストラクタ
	POSGRID3() {}
	POSGRID3(const int nx, const int ny, const int nz) { x = nx; y = ny; z = nz; }	// 代入

	// デストラクタ
	~POSGRID3() {}

	// メンバ関数
	bool operator==(const POSGRID3& grid) const;
	bool operator!=(const POSGRID3& grid) const;
	POSGRID3 operator+(const POSGRID3& grid) const;
	POSGRID3 operator-(const POSGRID3& grid) const;
	POSGRID3 operator*(const int nVal) const;
	POSGRID3 operator/(const int nVal) const;

	// メンバ変数
	int x;	// 方眼座標 (x)
	int y;	// 方眼座標 (y)
	int z;	// 方眼座標 (z)
};

//************************************************************
//	名前空間宣言
//************************************************************
// 便利関数空間
namespace useful
{
	// 通常関数
	void DivideDigitNum		// 数値の桁数ごとの分解
	( // 引数
		int *pNumDivide,	// 分解結果の格納配列
		const int nNum,		// 分解する数値
		const int nMaxDigit	// 分解する数字の桁数
	);
	void NormalizeNormal	// 法線の正規化
	( // 引数
		const D3DXVECTOR3& rPosLeft,	// 左位置
		const D3DXVECTOR3& rPosCenter,	// 中心位置
		const D3DXVECTOR3& rPosRight,	// 右位置
		D3DXVECTOR3& rNor				// 法線
	);
	void ReplaceConsecChar	// 文字列内の連続文字の置換
	( // 引数
		std::string *pDestStr,		// 置き換えを行う文字列
		const char cRepChar,		// 検出する文字
		const std::string& rRepStr,	// 置き換える文字列
		const int nRepStrLength		// 置き換える文字列の長さ (通常はstringのsize)
	);

	D3DXVECTOR2 CalcPosParabola		// 経過時間・X移動量・重力から放物線の位置を求める処理
	( // 引数
		const float fGravity,		// 重力
		const float fMoveX,			// X移動量
		const float fDestPosX,		// 最大X座標
		const float fDestPosY,		// 最大Y座標
		const float fTime,			// 経過時間
		float *pMaxTime = nullptr,	// 最大経過時間
		float *pMaxPosY = nullptr	// 最大到達Y座標
	);
	D3DXVECTOR2 CalcMoveParabola	// 経過時間・X移動量・重力から放物線の移動量を求める処理
	( // 引数
		const float fGravity,		// 重力
		const float fMoveX,			// X移動量
		const float fDestPosX,		// 最大X座標
		const float fDestPosY,		// 最大Y座標
		const float fTime,			// 経過時間
		float *pMaxTime = nullptr,	// 最大経過時間
		float *pMaxPosY = nullptr	// 最大到達Y座標
	);

	float RandomRot(void);			// ランダム向き取得
	void NormalizeRot(float& rRot);	// 向きの正規化
	void NormalizeRot(D3DXVECTOR3& rRot);			// 三軸向きの正規化
	void PathToBaseName(std::string *pPath);		// パスのベースネーム変換
	void StandardizePathPart(std::string *pPath);	// パス区切りの標準化
	void VecToRot(const D3DXVECTOR3& rVec, float *pPhi, float *pTheta);		// ベクトルの向き変換
	void RotToVec(const float fPhi, const float fTheta, D3DXVECTOR3 *pVec);	// 向きのベクトル変換
	float GetTexWidthFromAspect(const float fHeight, const int nTexID);		// 縦幅からアスペクト比を考慮した横幅取得
	float GetTexHeightFromAspect(const float fWidth, const int nTexID);		// 横幅からアスペクト比を考慮した縦幅取得
	D3DXVECTOR3 GetMatrixPosition(const D3DXMATRIX& rMtx);	// マトリックス位置取得
	D3DXVECTOR3 GetMatrixRotation(const D3DXMATRIX& rMtx);	// マトリックス向き取得
	D3DXVECTOR3 GetMatrixScaling(const D3DXMATRIX& rMtx);	// マトリックス拡大率取得

	// テンプレート関数
	template<class T> T *ZeroClear(T *pClear);	// ゼロクリア
	template<class T> bool LimitNum		// 値の範囲内制限
	( // 引数
		T& rNum,		// 制限数値
		const T min,	// 最小範囲
		const T max		// 最大範囲
	);
	template<class T> bool LimitMinNum	// 値の最小値制限
	( // 引数
		T& rNum,		// 制限数値
		const T min		// 最小範囲
	);
	template<class T> bool LimitMaxNum	// 値の最大値制限
	( // 引数
		T& rNum,		// 制限数値
		const T max		// 最大範囲
	);
	template<class T> float ValueToRate	// 値の割合変換
	( // 引数
		const T num,	// 割合化する数値
		const T min,	// 最小範囲
		const T max		// 最大範囲
	);
	template<class T> T RateToValue		// 割合の値変換
	( // 引数
		const float fRate,	// 値化する数値
		const T min,		// 最小範囲
		const T max			// 最大範囲
	);
	template<class T> void SortNum		// 値のソート
	( // 引数
		T *pSortNum,		// ソート配列
		const int nMaxKeep	// 配列サイズ
	);
	template<class T> void Shuffle		// シャッフル
	(
		T *pShuffleData,		// シャッフル配列
		const int nMaxShuffle	// 配列サイズ
	);
}

// イージング関数空間
namespace easeing
{
	// 通常関数
	inline float Liner(const float x)		{ return x; }

	inline float InSine(const float x)		{ return 1.0f - cosf((x * D3DX_PI) * 0.5f); }
	inline float OutSine(const float x)		{ return sinf((x * D3DX_PI) * 0.5f); }
	inline float InOutSine(const float x)	{ return -(cosf(x * D3DX_PI) - 1.0f) * 0.5f; }

	inline float InQuad(const float x)		{ return x * x; }
	inline float OutQuad(const float x)		{ return 1.0f - (1.0f - x) * (1.0f - x); }
	inline float InOutQuad(const float x)	{ return (x < 0.5f) ? (2.0f * x * x) : (1.0f - powf(-2.0f * x + 2.0f, 2.0f) * 0.5f); }

	inline float InCubic(const float x)		{ return x * x * x; }
	inline float OutCubic(const float x)	{ return 1.0f - powf(1.0f - x, 3.0f); }
	inline float InOutCubic(const float x)	{ return (x < 0.5f) ? (4.0f * x * x * x) : (1.0f - powf(-2.0f * x + 2.0f, 3.0f) * 0.5f); }

	inline float InQuart(const float x)		{ return x * x * x * x; }
	inline float OutQuart(const float x)	{ return 1.0f - powf(1.0f - x, 4.0f); }
	inline float InOutQuart(const float x)	{ return (x < 0.5f) ? (8.0f * x * x * x * x) : (1.0f - powf(-2.0f * x + 2.0f, 4.0f) * 0.5f); }

	inline float InQuint(const float x)		{ return x * x * x * x * x; }
	inline float OutQuint(const float x)	{ return 1.0f - powf(1.0f - x, 5.0f); }
	inline float InOutQuint(const float x)	{ return (x < 0.5f) ? (16.0f * x * x * x * x * x) : (1.0f - powf(-2.0f * x + 2.0f, 5.0f) * 0.5f); }

	// テンプレート関数
	template<class T> inline float Liner(T num, const T min, const T max)		{ return Liner(useful::ValueToRate(num, min, max)); }

	template<class T> inline float InSine(T num, const T min, const T max)		{ return InSine(useful::ValueToRate(num, min, max)); }
	template<class T> inline float OutSine(T num, const T min, const T max)		{ return OutSine(useful::ValueToRate(num, min, max)); }
	template<class T> inline float InOutSine(T num, const T min, const T max)	{ return InOutSine(useful::ValueToRate(num, min, max)); }

	template<class T> inline float InQuad(T num, const T min, const T max)		{ return InQuad(useful::ValueToRate(num, min, max)); }
	template<class T> inline float OutQuad(T num, const T min, const T max)		{ return OutQuad(useful::ValueToRate(num, min, max)); }
	template<class T> inline float InOutQuad(T num, const T min, const T max)	{ return InOutQuad(useful::ValueToRate(num, min, max)); }

	template<class T> inline float InCubic(T num, const T min, const T max)		{ return InCubic(useful::ValueToRate(num, min, max)); }
	template<class T> inline float OutCubic(T num, const T min, const T max)	{ return OutCubic(useful::ValueToRate(num, min, max)); }
	template<class T> inline float InOutCubic(T num, const T min, const T max)	{ return InOutCubic(useful::ValueToRate(num, min, max)); }

	template<class T> inline float InQuart(T num, const T min, const T max)		{ return InQuart(useful::ValueToRate(num, min, max)); }
	template<class T> inline float OutQuart(T num, const T min, const T max)	{ return OutQuart(useful::ValueToRate(num, min, max)); }
	template<class T> inline float InOutQuart(T num, const T min, const T max)	{ return InOutQuart(useful::ValueToRate(num, min, max)); }

	template<class T> inline float InQuint(T num, const T min, const T max)		{ return InQuint(useful::ValueToRate(num, min, max)); }
	template<class T> inline float OutQuint(T num, const T min, const T max)	{ return OutQuint(useful::ValueToRate(num, min, max)); }
	template<class T> inline float InOutQuint(T num, const T min, const T max)	{ return InOutQuint(useful::ValueToRate(num, min, max)); }
}

// マテリアル関数空間
namespace material
{
	// 通常関数
	D3DXMATERIAL White(void);		// 白色マテリアル取得
	D3DXMATERIAL Red(void);			// 赤色マテリアル取得
	D3DXMATERIAL Green(void);		// 緑色マテリアル取得
	D3DXMATERIAL Blue(void);		// 青色マテリアル取得
	D3DXMATERIAL Yellow(void);		// 黄色マテリアル取得
	D3DXMATERIAL Cyan(void);		// 水色マテリアル取得
	D3DXMATERIAL DamageRed(void);	// ダメージ赤色マテリアル取得
	D3DXMATERIAL GlowGreen(void);	// 発光緑色マテリアル取得
	D3DXMATERIAL GlowCyan(void);	// 発光水色マテリアル取得
}

//************************************************************
//	テンプレート関数
//************************************************************
//	便利関数空間
//************************************************************
//============================================================
//	ゼロクリア
//============================================================
template<class T> T *useful::ZeroClear(T *pClear)
{
	// 引数情報をゼロクリア
	memset(pClear, 0, sizeof(*pClear));

	// クリア後のアドレスを返す
	return pClear;
}

//============================================================
//	値の範囲内制限処理
//============================================================
template<class T> bool useful::LimitNum
(
	T& rNum,		// 制限数値
	const T min,	// 最小範囲
	const T max		// 最大範囲
)
{
	if (rNum < min)
	{ // 制限数値が最小範囲を超えていた場合

		// 範囲内に補正
		rNum = min;

		// 真を返す
		return true;
	}
	else if (rNum > max)
	{ // 制限数値が最大範囲を超えていた場合

		// 範囲内に補正
		rNum = max;

		// 真を返す
		return true;
	}

	// 偽を返す
	return false;
}

//============================================================
//	値の最小値制限処理
//============================================================
template<class T> bool useful::LimitMinNum
(
	T& rNum,	// 制限数値
	const T min	// 最小範囲
)
{
	if (rNum < min)
	{ // 制限数値が最小範囲を超えていた場合

		// 範囲内に補正
		rNum = min;

		// 真を返す
		return true;
	}

	// 偽を返す
	return false;
}

//============================================================
//	値の最大値制限処理
//============================================================
template<class T> bool useful::LimitMaxNum
(
	T& rNum,	// 制限数値
	const T max	// 最大範囲
)
{
	if (rNum > max)
	{ // 制限数値が最大範囲を超えていた場合

		// 範囲内に補正
		rNum = max;

		// 真を返す
		return true;
	}

	// 偽を返す
	return false;
}

//============================================================
//	値の割合変換
//============================================================
template<class T> float useful::ValueToRate
(
	const T num,	// 割合化する数値
	const T min,	// 最小範囲
	const T max		// 最大範囲
)
{
	// 割る数を求める
	float fDiv = static_cast<float>(max) - static_cast<float>(min);
	if (fDiv == 0.0f) { return 0.0f; }	// 0除算対策

	// 割合変換した値を返す
	return (static_cast<float>(num) - static_cast<float>(min)) / fDiv;
}

//============================================================
//	割合の値変換
//============================================================
template<class T> T useful::RateToValue
(
	const float fRate,	// 値化する数値
	const T min,		// 最小範囲
	const T max			// 最大範囲
)
{
	// 値変換した割合を返す
	return static_cast<T>((fRate * (max - min)) + min);
}

//============================================================
//	ソート処理
//============================================================
template<class T> void useful::SortNum
(
	T *pSortNum,		// ソート配列
	const int nMaxKeep	// 配列サイズ
)
{
	// 変数を宣言
	T	keepNum;		// ソート用
	int	nCurrentMaxID;	// 最大値のインデックス

	for (int nCntKeep = 0; nCntKeep < (nMaxKeep - 1); nCntKeep++)
	{ // 入れ替える値の総数 -1回分繰り返す

		// 現在の繰り返し数を代入 (要素1とする)
		nCurrentMaxID = nCntKeep;

		for (int nCntSort = nCntKeep + 1; nCntSort < nMaxKeep; nCntSort++)
		{ // 入れ替える値の総数 -nCntKeep分繰り返す

			if (pSortNum[nCurrentMaxID] < pSortNum[nCntSort])
			{ // 最大値に設定されている値より、現在の値のほうが大きい場合

				// 現在の要素番号を最大値に設定
				nCurrentMaxID = nCntSort;
			}
		}

		if (nCntKeep != nCurrentMaxID)
		{ // 最大値の要素番号に変動があった場合

			// 要素の入れ替え
			keepNum					= pSortNum[nCntKeep];
			pSortNum[nCntKeep]		= pSortNum[nCurrentMaxID];
			pSortNum[nCurrentMaxID]	= keepNum;
		}
	}
}

//============================================================
//	シャッフル処理
//============================================================
template<class T> void useful::Shuffle
(
	T *pShuffleData,		// シャッフル配列
	const int nMaxShuffle	// 配列サイズ
)
{
	for (int nCntShu = nMaxShuffle - 1; nCntShu > 0; nCntShu--)
	{ // 引数の総数分繰り返す

		// 変数を宣言
		T swap;		// 要素の入れ替え用
		int nRandom = rand() % nCntShu;	// 要素を入れ替えるインデックス

		// 要素の入れ替え
		swap					= pShuffleData[nRandom];
		pShuffleData[nRandom]	= pShuffleData[nCntShu];
		pShuffleData[nCntShu]	= swap;
	}
}

#endif	// _USEFUL_H_
