//============================================================
//
//	キャラクター処理 [character.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "character.h"
#include "manager.h"
#include "renderer.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *LOAD_FOLDER = "data\\CHARACTER";	// キャラクターフォルダ相対パス
	const CCharacter::SCharaData ZERO_CHARADATA;	// キャラクター情報初期値
}

//************************************************************
//	親クラス [CCharacter] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CCharacter::CCharacter()
{
	// キャラクター連想配列をクリア
	m_mapCharacter.clear();
}

//============================================================
//	デストラクタ
//============================================================
CCharacter::~CCharacter()
{

}

//============================================================
//	キャラクター初期化処理
//============================================================
HRESULT CCharacter::Init(void)
{
	// キャラクター連想配列を初期化
	m_mapCharacter.clear();

	// 成功を返す
	return S_OK;
}

//============================================================
//	キャラクター終了処理
//============================================================
void CCharacter::Uninit(void)
{
	for (auto& rMap : m_mapCharacter)
	{ // キャラクター数分繰り返す

		for (auto& rMotionInfo : rMap.second.infoMotion.vecMotion)
		{ // モーション情報の要素数分繰り返す

			for (auto& rKeyInfo : rMotionInfo.vecKey)
			{ // キー情報の要素数分繰り返す

				// パーツ情報をクリア
				rKeyInfo.vecParts.clear();
			}

			// キー情報をクリア
			rMotionInfo.vecKey.clear();
		}

		// モーション情報をクリア
		rMap.second.infoMotion.vecMotion.clear();

		// パーツ情報をクリア
		rMap.second.infoParts.vecParts.clear();
	}

	// キャラクター連想配列をクリア
	m_mapCharacter.clear();
}

//============================================================
//	キャラクター全読込処理
//============================================================
HRESULT CCharacter::LoadAll(void)
{
	// キャラクターの全読込
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
//	キャラクター登録
//============================================================
CCharacter::SCharaData CCharacter::Regist(const char *pCharaPass)
{
	// 既に生成済みかを検索
	auto itr = m_mapCharacter.find(pCharaPass);	// 引数のキャラクター情報を検索
	if (itr != m_mapCharacter.end())
	{ // 生成済みの場合

		// 読込済みのキャラクター情報を返す
		return itr->second;
	}

	// キャラクター情報を読込
	SCharaData tempCharaData;	// キャラクター情報
	if (FAILED(LoadSetup(&tempCharaData, pCharaPass)))
	{ // 読込に失敗した場合

		// 初期値を返す
		assert(false);
		return ZERO_CHARADATA;
	}

	// キャラクター情報を保存
	m_mapCharacter.insert(std::make_pair(pCharaPass, tempCharaData));

	// 生成したキャラクター情報を返す
	return tempCharaData;
}

//============================================================
//	生成処理
//============================================================
CCharacter *CCharacter::Create(void)
{
	// キャラクターの生成
	CCharacter *pCharacter = new CCharacter;
	if (pCharacter == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// キャラクターの初期化
		if (FAILED(pCharacter->Init()))
		{ // キャラクター初期化に失敗した場合

			// キャラクターの破棄
			SAFE_DELETE(pCharacter);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pCharacter;
	}
}

//============================================================
//	破棄処理
//============================================================
void CCharacter::Release(CCharacter *&prCharacter)
{
	// キャラクターの終了
	assert(prCharacter != nullptr);
	prCharacter->Uninit();

	// メモリ開放
	SAFE_DELETE(prCharacter);
}

//============================================================
//	フォルダ全検索処理
//============================================================
HRESULT CCharacter::SearchFolderAll(std::string sFolderPath)
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

//============================================================
//	キャラクター情報セットアップ処理
//============================================================
HRESULT CCharacter::LoadSetup(SCharaData *pInfoChara, const char *pCharaPass)
{
	SPartsInfo *pInfoParts = &pInfoChara->infoParts;	// パーツ情報
	D3DXVECTOR3 pos = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用
	int nID = 0;	// インデックスの代入用

	// ファイルを開く
	std::ifstream file(pCharaPass);	// ファイルストリーム
	if (file.fail())
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(nullptr, "キャラクターセットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);

		// 失敗を返す
		return E_FAIL;
	}

	// ファイルを読込
	std::string str;	// 読込文字列
	while (file >> str)
	{ // ファイルの終端ではない場合ループ

		if (str.front() == '#')
		{ // コメントアウトされている場合

			// 一行全て読み込む
			std::getline(file, str);
		}
		else if (str == "CHARACTERSET")
		{
			do
			{ // END_CHARACTERSETを読み込むまでループ

				// 文字列を読み込む
				file >> str;

				if (str.front() == '#')
				{ // コメントアウトされている場合

					// 一行全て読み込む
					std::getline(file, str);
				}
				else if (str == "PARTSSET")
				{
					do
					{ // END_PARTSSETを読み込むまでループ

						// 文字列を読み込む
						file >> str;

						if (str == "INDEX")
						{
							file >> str;	// ＝を読込
							file >> nID;	// モデルのインデックスを読込

							// 空の要素を最後尾に追加
							pInfoParts->vecParts.emplace_back();
						}
						else if (str == "PARENT")
						{
							file >> str;									// ＝を読込
							file >> pInfoParts->vecParts[nID].nParentID;	// モデルの親インデックスを読込
						}
						else if (str == "POS")
						{
							file >> str;								// ＝を読込
							file >> pInfoParts->vecParts[nID].pos.x;	// X位置オフセットを読込
							file >> pInfoParts->vecParts[nID].pos.y;	// Y位置オフセットを読込
							file >> pInfoParts->vecParts[nID].pos.z;	// Z位置オフセットを読込
						}
						else if (str == "ROT")
						{
							file >> str;								// ＝を読込
							file >> pInfoParts->vecParts[nID].rot.x;	// X向きオフセットを読込
							file >> pInfoParts->vecParts[nID].rot.y;	// Y向きオフセットを読込
							file >> pInfoParts->vecParts[nID].rot.z;	// Z向きオフセットを読込
						}
						else if (str == "FILEPASS")
						{
							file >> str;								// ＝を読込
							file >> pInfoParts->vecParts[nID].strPass;	// モデルパスを読込

							// モデルパスを標準化
							useful::StandardizePathPart(&pInfoParts->vecParts[nID].strPass);
						}
					} while (str != "END_PARTSSET");	// END_PARTSSETを読み込むまでループ
				}
			} while (str != "END_CHARACTERSET");	// END_CHARACTERSETを読み込むまでループ
		}
		else if (str == "MOTIONPASS")
		{
			file >> str;	// ＝を読込
			file >> str;	// モーションパスを読込

			// モーションパスを読み込む
			LoadMotionSetup(&pInfoChara->infoMotion, pInfoParts, str.c_str());
		}
	}

	// ファイルを閉じる
	file.close();

	// 成功を返す
	return S_OK;
}

//============================================================
//	モーション情報セットアップ処理
//============================================================
HRESULT CCharacter::LoadMotionSetup(CMotion::SInfo *pInfoMotion, const SPartsInfo *pInfoParts, const char *pMotionPass)
{
	D3DXVECTOR3 pos = VEC3_ZERO;	// 読込位置
	D3DXVECTOR3 rot = VEC3_ZERO;	// 読込向き
	int nCurKey		= 0;			// 現在のキー番号
	int nCurParts	= 0;			// 現在のパーツ番号
	int nCastBool	= 0;			// bool型変換

	// ファイルを開く
	std::ifstream file(pMotionPass);	// ファイルストリーム
	if (file.fail())
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(nullptr, "モーションセットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);

		// 失敗を返す
		return E_FAIL;
	}

	// ファイルを読込
	std::string str;	// 読込文字列
	while (file >> str)
	{ // ファイルの終端ではない場合ループ

		if (str.front() == '#')
		{ // コメントアウトされている場合

			// 一行全て読み込む
			std::getline(file, str);
		}
		else if (str == "MOTIONSET")
		{
			int nType = pInfoMotion->GetNumMotion();	// 現在のモーション数
			pInfoMotion->vecMotion.emplace_back();		// 空の要素を最後尾に追加
			CMotion::SMotion *pMotion = &pInfoMotion->vecMotion[nType];	// 現在のモーション情報

			// 現在のキー番号を初期化
			nCurKey = 0;

			do
			{ // END_MOTIONSETを読み込むまでループ

				// 文字列を読み込む
				file >> str;

				if (str.front() == '#')
				{ // コメントアウトされている場合

					// 一行全て読み込む
					std::getline(file, str);
				}
				else if (str == "WEAPON")
				{
					file >> str;		// ＝を読込
					file >> nCastBool;	// 武器表示のON/OFFを読込

					// 読み込んだ値をbool型に変換
					pMotion->bWeaponDisp = (nCastBool != 0);
				}
				else if (str == "LOOP")
				{
					file >> str;		// ＝を読込
					file >> nCastBool;	// ループのON/OFFを読込

					// 読み込んだ値をbool型に変換
					pMotion->bLoop = (nCastBool != 0);
				}
				else if (str == "CANCEL")
				{
					file >> str;					// ＝を読込
					file >> pMotion->nCancelFrame;	// キャンセル可能フレームを読込
				}
				else if (str == "COMBO")
				{
					file >> str;					// ＝を読込
					file >> pMotion->nComboFrame;	// コンボ可能フレームを読込
				}
				else if (str == "LEFT_COLL")
				{
					file >> str;						// ＝を読込
					file >> pMotion->collLeft.nMin;		// 判定を出す開始フレームを読込
					file >> pMotion->collLeft.nMax;		// 判定を消す終了フレームを読込
				}
				else if (str == "RIGHT_COLL")
				{
					file >> str;						// ＝を読込
					file >> pMotion->collRight.nMin;	// 判定を出す開始フレームを読込
					file >> pMotion->collRight.nMax;	// 判定を消す終了フレームを読込
				}
				else if (str == "KEYSET")
				{
					pMotion->vecKey.emplace_back();	// 空の要素を最後尾に追加
					CMotion::SKey *pKey = &pMotion->vecKey[nCurKey];	// 現在のキー情報

					// 現在のパーツ番号を初期化
					nCurParts = 0;

					do
					{ // END_KEYSETを読み込むまでループ

						// 文字列を読み込む
						file >> str;

						if (str.front() == '#')
						{ // コメントアウトされている場合

							// 一行全て読み込む
							std::getline(file, str);
						}
						else if (str == "FRAME")
						{
							file >> str;			// ＝を読込
							file >> pKey->nFrame;	// キーが切り替わるまでのフレーム数を読込
						}
						else if (str == "MOVE")
						{
							file >> str;			// ＝を読込
							file >> pKey->move.x;	// キーの移動量Xを読込
							file >> pKey->move.y;	// キーの移動量Yを読込
							file >> pKey->move.z;	// キーの移動量Xを読込
						}
						else if (str == "KEY")
						{
							pKey->vecParts.emplace_back();	// 空の要素を最後尾に追加
							CMotion::SParts *pParts = &pKey->vecParts[nCurParts];	// 現在のパーツ情報

							do
							{ // END_KEYを読み込むまでループ

								// 文字列を読み込む
								file >> str;

								if (str.front() == '#')
								{ // コメントアウトされている場合

									// 一行全て読み込む
									std::getline(file, str);
								}
								else if (str == "POS")
								{
									file >> str;			// ＝を読込
									file >> pParts->pos.x;	// X位置を読込
									file >> pParts->pos.y;	// Y位置を読込
									file >> pParts->pos.z;	// Z位置を読込

									// 読み込んだ位置にパーツの初期位置を加算
									pParts->pos += pInfoParts->vecParts[nCurParts].pos;
								}
								else if (str == "ROT")
								{
									file >> str;			// ＝を読込
									file >> pParts->rot.x;	// X向きを読込
									file >> pParts->rot.y;	// Y向きを読込
									file >> pParts->rot.z;	// Z向きを読込

									// 読み込んだ向きにパーツの初期向きを加算
									pParts->rot += pInfoParts->vecParts[nCurParts].rot;
									useful::NormalizeRot(pParts->rot);	// 向き正規化
								}
							} while (str != "END_KEY");	// END_KEYを読み込むまでループ

							// 現在のパーツ番号を加算
							nCurParts++;
						}
					} while (str != "END_KEYSET");	// END_KEYSETを読み込むまでループ

					// 現在のキー番号を加算
					nCurKey++;
				}
			} while (str != "END_MOTIONSET");	// END_MOTIONSETを読み込むまでループ
		}
	}

	// ファイルを閉じる
	file.close();

	// 成功を返す
	return S_OK;
}
