//============================================================
//
//	キャラクターヘッダー [character.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _CHARACTER_H_
#define _CHARACTER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "motion.h"

//************************************************************
//	クラス定義
//************************************************************
// キャラクタークラス
class CCharacter
{
public:
	// コンストラクタ
	CCharacter();

	// デストラクタ
	~CCharacter();

	// パーツ構造体
	struct SParts
	{
		// コンストラクタ
		SParts() :
			pos			(VEC3_ZERO),	// 位置
			rot			(VEC3_ZERO),	// 向き
			nParentID	(NONE_IDX)		// 親インデックス
		{
			strPass.clear();	// モデルパスをクリア
		}

		// メンバ変数
		std::string strPass;	// モデルパス
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		int nParentID;			// 親インデックス
	};

	// パーツ情報構造体
	struct SPartsInfo
	{
		// コンストラクタ
		SPartsInfo()
		{
			vecParts.clear();	// パーツ情報をクリア
		}

		// メンバ関数
		int GetNumParts(void) { return vecParts.size(); }	// パーツ情報の総数取得

		// メンバ変数
		std::vector<SParts> vecParts;	// パーツ情報
	};

	// キャラクター情報構造体
	struct SCharaData
	{
		// コンストラクタ
		SCharaData() {}

		// メンバ変数
		CMotion::SInfo infoMotion;	// モーション情報
		SPartsInfo infoParts;		// パーツ情報
	};

	// メンバ関数
	HRESULT Init(void);		// キャラクター初期化
	void Uninit(void);		// キャラクター終了
	HRESULT LoadAll(void);	// キャラクター全読込
	SCharaData Regist(const char *pCharaPass);	// キャラクター登録

	// 静的メンバ関数
	static CCharacter *Create(void);	// 生成
	static void Release(CCharacter *&prCharacter);	// 破棄

private:
	// メンバ関数
	HRESULT SearchFolderAll(std::string sFolderPath);	// フォルダ全検索

	// メンバ関数
	HRESULT LoadSetup(SCharaData *pInfoChara, const char *pCharaPass);	// キャラクター情報セットアップ
	HRESULT LoadMotionSetup(CMotion::SInfo *pInfoMotion, const SPartsInfo *pInfoParts, const char *pMotionPass);	// モーション情報セットアップ

	// メンバ変数
	std::map<std::string, SCharaData> m_mapCharacter;	// キャラクター連想配列
};

#endif	// _CHARACTER_H_
