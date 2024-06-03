//============================================================
//
//	リストマネージャーヘッダー [listManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _LIST_MANAGER_H_
#define _LIST_MANAGER_H_

//************************************************************
//	クラス定義
//************************************************************
// リストマネージャークラス
template<class T> class CListManager
{
public:
	// コンストラクタ
	CListManager();

	// デストラクタ
	~CListManager();

	// イテレーター型エイリアス定義
	using AIterator = std::_List_iterator<std::_List_val<std::_List_simple_types<T*>>>;

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	AIterator GetBegin(void);	// リスト先頭取得
	AIterator GetEnd(void);	// リスト最後尾取得
	AIterator GetIndex(const int nID);		// リストインデックス取得
	AIterator AddList(T *pObject);			// リスト追加
	void DelList(const AIterator iterator);	// リスト削除
	int GetNumAll(void);			// リスト内の要素数取得
	std::list<T*> GetList(void);	// リスト取得

	// 静的メンバ関数
	static CListManager *Create(void);	// 生成
	static void Release(CListManager *&prListManager);	// 破棄

private:
	// メンバ変数
	std::list<T*> m_list;	// オブジェクトリスト
};

//************************************************************
//	親クラス [CListManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
template<class T> CListManager<T>::CListManager()
{
	// オブジェクトリストの全要素を削除
	m_list.clear();
}

//============================================================
//	デストラクタ
//============================================================
template<class T> CListManager<T>::~CListManager()
{

}

//============================================================
//	初期化処理
//============================================================
template<class T> HRESULT CListManager<T>::Init(void)
{
	// オブジェクトリストの全要素を削除
	m_list.clear();

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
template<class T> void CListManager<T>::Uninit(void)
{
	// オブジェクトリストの全要素を削除
	m_list.clear();
}

//============================================================
//	リスト先頭取得処理
//============================================================
template<class T>
std::_List_iterator<std::_List_val<std::_List_simple_types<T*>>> CListManager<T>::GetBegin(void)
{
	// リスト先頭を返す
	return m_list.begin();
}

//============================================================
//	リスト最後尾取得処理
//============================================================
template<class T>
std::_List_iterator<std::_List_val<std::_List_simple_types<T*>>> CListManager<T>::GetEnd(void)
{
	// リスト最後尾を返す
	return m_list.end();
}

//============================================================
//	リストインデックス取得処理
//============================================================
template<class T>
std::_List_iterator<std::_List_val<std::_List_simple_types<T*>>> CListManager<T>::GetIndex(const int nID)
{
	AIterator itr = m_list.begin();	// リスト先頭

	// インデックスが範囲外の場合抜ける
	if (nID <= NONE_IDX || nID >= (int)m_list.size()) { assert(false); return itr; }

	// 引数のインデックス分進める
	std::advance(itr, nID);

	// 進めたイテレーターを返す
	return itr;
}

//============================================================
//	リスト追加処理
//============================================================
template<class T>
std::_List_iterator<std::_List_val<std::_List_simple_types<T*>>> CListManager<T>::AddList(T *pObject)
{
	// リストの最後尾にオブジェクトを追加し、イテレーターを返す
	return m_list.insert(m_list.end(), pObject);
}

//============================================================
//	リスト削除処理
//============================================================
template<class T> void CListManager<T>::DelList(const AIterator iterator)
{
	// 引数イテレーターの要素をリストから削除
	m_list.erase(iterator);
}

//============================================================
//	リスト内の要素数の取得処理
//============================================================
template<class T> int CListManager<T>::GetNumAll(void)
{
	// リスト内の要素数を返す
	return (int)m_list.size();
}

//============================================================
//	リスト取得処理
//============================================================
template<class T> std::list<T*> CListManager<T>::GetList(void)
{
	// オブジェクトリストを返す
	return m_list;
}

//============================================================
//	生成処理
//============================================================
template<class T> CListManager<T> *CListManager<T>::Create(void)
{
	// リストマネージャーの生成
	CListManager *pListManager = new CListManager;
	if (pListManager == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// リストマネージャーの初期化
		if (FAILED(pListManager->Init()))
		{ // 初期化に失敗した場合

			// リストマネージャーの破棄
			SAFE_DELETE(pListManager);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pListManager;
	}
}

//============================================================
//	破棄処理
//============================================================
template<class T> void CListManager<T>::Release(CListManager *&prListManager)
{
	// リストマネージャーの終了
	assert(prListManager != nullptr);
	prListManager->Uninit();

	// メモリ開放
	SAFE_DELETE(prListManager);
}

#endif	// _LIST_MANAGER_H_
