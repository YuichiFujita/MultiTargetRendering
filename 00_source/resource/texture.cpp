//============================================================
//
//	テクスチャ処理 [texture.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "texture.h"
#include "manager.h"
#include "renderer.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *LOAD_FOLDER = "data\\TEXTURE";	// テクスチャフォルダ相対パス

	const D3DXVECTOR3 INIT_VTXMIN = D3DXVECTOR3( 9999.0f,  9999.0f,  9999.0f);	// モデルの最小の頂点座標の初期値
	const D3DXVECTOR3 INIT_VTXMAX = D3DXVECTOR3(-9999.0f, -9999.0f, -9999.0f);	// モデルの最大の頂点座標の初期値
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
int CTexture::m_nNumAll = 0;	// テクスチャの総数

//************************************************************
//	親クラス [CTexture] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CTexture::CTexture()
{
	// テクスチャ連想配列をクリア
	m_mapTexture.clear();
}

//============================================================
//	デストラクタ
//============================================================
CTexture::~CTexture()
{

}

//============================================================
//	テクスチャ初期化処理
//============================================================
HRESULT CTexture::Init(void)
{
	// テクスチャ連想配列を初期化
	m_mapTexture.clear();

	// 成功を返す
	return S_OK;
}

//============================================================
//	テクスチャ終了処理
//============================================================
void CTexture::Uninit(void)
{
	for (auto& rMap : m_mapTexture)
	{ // テクスチャの要素数分繰り返す

		// テクスチャの破棄
		SAFE_RELEASE(rMap.second.textureData.pTexture);
	}

	// テクスチャ連想配列をクリア
	m_mapTexture.clear();
}

//============================================================
//	テクスチャ全読込処理
//============================================================
HRESULT CTexture::LoadAll(void)
{
	// テクスチャの全読込
	if (FAILED(SearchFolderAll(LOAD_FOLDER)))
	{ // 読込に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	テクスチャ登録処理 (生成)
//============================================================
int CTexture::Regist(const SInfo info)
{
	// 変数を宣言
	HRESULT  hr;			// 異常終了の確認用
	SMapInfo tempMapInfo;	// マップ情報
	int nID = m_nNumAll;	// テクスチャ読込番号

	// マップ情報のポインタを初期化
	tempMapInfo.textureData.pTexture = nullptr;	// テクスチャへのポインタ

	// 空のテクスチャを生成
	hr = D3DXCreateTexture
	( // 引数
		GET_DEVICE,		// Direct3Dデバイス
		info.Width,		// テクスチャ横幅
		info.Height,	// テクスチャ縦幅
		info.MipLevels,	// ミップマップレベル
		info.Usage,		// 性質・確保オプション
		info.Format,	// ピクセルフォーマット
		info.Pool,		// 格納メモリ
		&tempMapInfo.textureData.pTexture	// テクスチャへのポインタ
	);
	if (FAILED(hr))
	{ // テクスチャの生成に失敗した場合

		// 失敗を返す
		assert(false);
		return NONE_IDX;
	}

	// テクスチャステータスを設定
	D3DXIMAGE_INFO *pStatus = &tempMapInfo.textureData.status;	// ステータス情報
	pStatus->Width			 = info.Width;						// テクスチャ横幅
	pStatus->Height			 = info.Height;						// テクスチャ縦幅
	pStatus->Depth			 = 1;								// テクスチャ深度
	pStatus->MipLevels		 = info.MipLevels;					// ミップマップレベル
	pStatus->Format			 = info.Format;						// ピクセルフォーマット
	pStatus->ResourceType	 = D3DRTYPE_TEXTURE;				// リソース種類
	pStatus->ImageFileFormat = (D3DXIMAGE_FILEFORMAT)NONE_IDX;	// ファイル形式 (作成のため無し)

	// ファイルパス名を保存
	tempMapInfo.sFilePassName = NONE_STRING;	// 読込ではないのでパス無し

	// アスペクト比を計算
	tempMapInfo.textureData.aspect.x = (float)info.Width / (float)info.Height;
	tempMapInfo.textureData.aspect.y = (float)info.Height / (float)info.Width;

	// テクスチャ情報を保存
	m_mapTexture.insert(std::make_pair(m_nNumAll, tempMapInfo));

	// テクスチャの総数を加算
	m_nNumAll++;

	// 読み込んだテクスチャの配列番号を返す
	return nID;
}

//============================================================
//	テクスチャ登録処理 (パス)
//============================================================
int CTexture::Regist(std::string sFilePass)
{
	// 変数を宣言
	HRESULT  hr;			// 異常終了の確認用
	SMapInfo tempMapInfo;	// マップ情報
	int nID = m_nNumAll;	// テクスチャ読込番号

	// マップ情報のポインタを初期化
	tempMapInfo.textureData.pTexture = nullptr;	// テクスチャへのポインタ

	// ファイルパスを標準化
	useful::StandardizePathPart(&sFilePass);

	// 既に読み込んでいないかを確認
	int nCntTexture = 0;
	for (const auto& rMap : m_mapTexture)
	{ // テクスチャの要素数分繰り返す

		if (rMap.second.sFilePassName.compare(sFilePass) == 0)
		{ // 文字列が一致した場合

			// すでに読み込んでいるテクスチャの配列番号を返す
			return nCntTexture;
		}

		// 次のテクスチャ番号にする
		nCntTexture++;
	}

	// テクスチャの読込
	hr = D3DXCreateTextureFromFileEx
	( // 引数
		GET_DEVICE,			// Direct3Dデバイス
		sFilePass.c_str(),	// テクスチャファイルパス
		D3DX_DEFAULT,		// テクスチャ横幅
		D3DX_DEFAULT,		// テクスチャ縦幅
		0,					// ミップマップレベル
		0,					// 性質・確保オプション
		D3DFMT_A8R8G8B8,	// ピクセルフォーマット
		D3DPOOL_MANAGED,	// 格納メモリ
		D3DX_DEFAULT,		// フィルタ
		D3DX_DEFAULT,		// ミップマップフィルタ
		0,					// カラーキー
		&tempMapInfo.textureData.status,	// テクスチャステータスへのポインタ
		nullptr,							// テクスチャパレットへのポインタ
		&tempMapInfo.textureData.pTexture	// テクスチャへのポインタ
	);
	if (FAILED(hr))
	{ // テクスチャの読込に失敗した場合

		// 失敗を返す
		assert(false);
		return NONE_IDX;
	}

	// ファイルパス名を保存
	tempMapInfo.sFilePassName = sFilePass;

	// アスペクト比を計算
	D3DXIMAGE_INFO status = tempMapInfo.textureData.status;	// テクスチャステータス
	tempMapInfo.textureData.aspect.x = (float)status.Width / (float)status.Height;
	tempMapInfo.textureData.aspect.y = (float)status.Height / (float)status.Width;

	// テクスチャ情報を保存
	m_mapTexture.insert(std::make_pair(m_nNumAll, tempMapInfo));

	// テクスチャの総数を加算
	m_nNumAll++;

	// 読み込んだテクスチャの配列番号を返す
	return nID;
}

//============================================================
//	テクスチャ情報の取得処理
//============================================================
CTexture::STexture CTexture::GetInfo(const int nID)
{
	int nArray = (int)m_mapTexture.size();	// 配列要素数
	if (nID > NONE_IDX && nID < nArray)
	{ // テクスチャがある場合

		// 引数のテクスチャ情報を返す
		return m_mapTexture.find(nID)->second.textureData;
	}
	else
	{ // テクスチャがない場合

		// インデックスエラー
		assert(false);

		if (nArray > 0)
		{ // テクスチャ生成がされている場合

			// 先頭テクスチャを返す
			return m_mapTexture.find(0)->second.textureData;
		}
		else
		{ // テクスチャが一つもない場合

			// 空のテクスチャ情報を返す
			STexture tempTexture;
			memset(&tempTexture, 0, sizeof(tempTexture));
			return tempTexture;
		}
	}
}

//============================================================
//	テクスチャポインタの取得処理
//============================================================
LPDIRECT3DTEXTURE9 CTexture::GetPtr(const int nID)
{
	if (nID >= 0 && nID < m_nNumAll)
	{ // 引数のインデックスが範囲内の場合

		if (nID > NONE_IDX && nID < (int)m_mapTexture.size())
		{ // テクスチャがある場合

			// 引数のテクスチャポインタを返す
			return m_mapTexture.find(nID)->second.textureData.pTexture;
		}
		else
		{ // テクスチャがない場合

			// nullptrを返す
			assert(false);
			return nullptr;
		}
	}
	else if (nID == NONE_IDX)
	{ // 引数のインデックスが -1の場合

		// nullptrを返す
		return nullptr;
	}
	else
	{ // 引数のインデックスが使用不可の場合

		// nullptrを返す
		assert(false);
		return nullptr;
	}
}

//============================================================
//	生成処理
//============================================================
CTexture *CTexture::Create(void)
{
	// テクスチャの生成
	CTexture *pTexture = new CTexture;
	if (pTexture == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// テクスチャの初期化
		if (FAILED(pTexture->Init()))
		{ // テクスチャ初期化に失敗した場合

			// テクスチャの破棄
			SAFE_DELETE(pTexture);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pTexture;
	}
}

//============================================================
//	破棄処理
//============================================================
void CTexture::Release(CTexture *&prTexture)
{
	// テクスチャの終了
	assert(prTexture != nullptr);
	prTexture->Uninit();

	// メモリ開放
	SAFE_DELETE(prTexture);
}

//============================================================
//	フォルダ全検索処理
//============================================================
HRESULT CTexture::SearchFolderAll(std::string sFolderPath)
{
	// 変数を宣言
	HANDLE hFile;	// 検索ハンドル
	WIN32_FIND_DATA findFileData;	// ファイル情報

	// 引数パスのディレクトリを取得
	std::string sAllLoadPath = sFolderPath + "\\*.*";	// 全読込パス
	hFile = FindFirstFile(sAllLoadPath.c_str(), &findFileData);
	if (INVALID_HANDLE_VALUE == hFile)
	{ // ハンドルが無効の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	do
	{ // ファイル内の情報全てを読み込む

		// 現在のディレクトリ、親ディレクトリの場合次のループに移行
		if (strcmp(findFileData.cFileName, ".") == 0)	{ continue; }
		if (strcmp(findFileData.cFileName, "..") == 0)	{ continue; }

		// ファイル名を相対パスに変換
		std::string sFullPath = sFolderPath;	// 現在の相対パスを設定
		sFullPath += "\\";						// パス区切り文字を追加
		sFullPath += findFileData.cFileName;	// ファイル名を追加

		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{ // ディレクトリだった場合

			// 新たなディレクトリを全検索
			SearchFolderAll(sFullPath);
		}
		else
		{ // ファイルだった場合

			// テクスチャを登録
			Regist(sFullPath.c_str());
		}

	} while (FindNextFile(hFile, &findFileData));	// 次のファイルを検索

	// 検索ハンドルを閉じる
	FindClose(hFile);

	// 成功を返す
	return S_OK;
}
