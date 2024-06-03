//============================================================
//
//	ランダムヘッダー [random.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _RANDOM_H_
#define _RANDOM_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include <random>
#include <algorithm>

//************************************************************
//	クラス定義
//************************************************************
// ランダムクラス
template<class T> class CRandom
{
public:
	// コンストラクタ
	CRandom();

	// デストラクタ
	~CRandom();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void AddList(T num, int nAdd = 1);	// 配列追加
	void AllDeleteList(void);			// 配列全削除
	T GetRandomNum(void);				// ランダム数値取得
	int GetNumAll(void);				// 配列内の要素数取得
	std::vector<T> GetList(void);		// 配列取得

	// 静的メンバ関数
	static CRandom *Create(void);	// 生成
	static void Release(CRandom *&prRandom);	// 破棄

private:
	// イテレーター型エイリアス定義
	using AIterator = std::_List_iterator<std::_List_val<std::_List_simple_types<T>>>;

	// メンバ変数
	std::vector<T> m_vector;	// オブジェクト配列
};

//************************************************************
//	親クラス [CRandom] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
template<class T> CRandom<T>::CRandom()
{
	// オブジェクト配列の全要素を削除
	m_vector.clear();
}

//============================================================
//	デストラクタ
//============================================================
template<class T> CRandom<T>::~CRandom()
{

}

//============================================================
//	初期化処理
//============================================================
template<class T> HRESULT CRandom<T>::Init(void)
{
	// オブジェクト配列の全要素を削除
	m_vector.clear();

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
template<class T> void CRandom<T>::Uninit(void)
{
	// オブジェクト配列の全要素を削除
	m_vector.clear();
}

//============================================================
//	配列追加処理
//============================================================
template<class T> void CRandom<T>::AddList(T num, int nAdd)
{
	// 加算数が自然数ではない場合抜ける
	if (nAdd <= 0) { return; }
	for (int nCntAdd = 0; nCntAdd < nAdd; nCntAdd++)
	{ // 追加数分繰り返す

		// 配列の最後尾にオブジェクトを追加
		m_vector.push_back(num);
	}
}

//============================================================
//	配列全削除処理
//============================================================
template<class T> void CRandom<T>::AllDeleteList(void)
{
	// オブジェクト配列の全要素を削除
	m_vector.clear();
}

//============================================================
//	ランダム数値取得処理
//============================================================
template<class T> T CRandom<T>::GetRandomNum(void)
{
	int nNumArray = (int)(m_vector.size());	// 配列の要素数
	if (nNumArray > 0)
	{ // 要素が設定されている場合

		// 乱数生成器を作成
		std::random_device rd;
		std::mt19937 gen(rd());

		// 要素をシャッフルする
		std::shuffle(m_vector.begin(), m_vector.end(), gen);

		// 使用する要素番号を決める
		int nRandHit = rand() % nNumArray;

		// ランダムで抽選された値を返す
		auto numRand = m_vector.begin();	// 配列の先頭
		numRand += nRandHit;				// 要素番号分イテレーター加算
		return *numRand;
	}

	// 初期値を返す
	T error;
	memset(&error, 0, sizeof(error));
	return error;
}

//============================================================
//	配列内の要素数の取得処理
//============================================================
template<class T> int CRandom<T>::GetNumAll(void)
{
	// 配列内の要素数を返す
	return (int)m_vector.size();
}

//============================================================
//	配列取得処理
//============================================================
template<class T> std::vector<T> CRandom<T>::GetList(void)
{
	// オブジェクト配列を返す
	return m_vector;
}

//============================================================
//	生成処理
//============================================================
template<class T> CRandom<T> *CRandom<T>::Create(void)
{
	// ランダムの生成
	CRandom *pRandom = new CRandom;
	if (pRandom == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// ランダムの初期化
		if (FAILED(pRandom->Init()))
		{ // 初期化に失敗した場合

			// ランダムの破棄
			SAFE_DELETE(pRandom);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pRandom;
	}
}

//============================================================
//	破棄処理
//============================================================
template<class T> void CRandom<T>::Release(CRandom *&prRandom)
{
	// ランダムの終了
	assert(prRandom != nullptr);
	prRandom->Uninit();

	// メモリ開放
	SAFE_DELETE(prRandom);
}

#endif	// _RANDOM_H_
