//============================================================
//
//	モデルヘッダー [model.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MODEL_H_
#define _MODEL_H_

//************************************************************
//	クラス定義
//************************************************************
// モデルクラス
class CModel
{
public:
	// コンストラクタ
	CModel();

	// デストラクタ
	~CModel();

	// モデル構造体
	struct SModel
	{
		LPD3DXMESH		pMesh;			// メッシュ (頂点情報) へのポインタ
		LPD3DXBUFFER	pBuffMat;		// マテリアルへのポインタ
		DWORD			dwNumMat;		// マテリアルの数
		D3DXVECTOR3		vtxMin;			// 最小の頂点座標
		D3DXVECTOR3		vtxMax;			// 最大の頂点座標
		D3DXVECTOR3		size;			// 大きさ
		float			fRadius;		// 半径
		int				*pTextureID;	// テクスチャインデックス
	};

	// マップ情報構造体
	struct SMapInfo
	{
		SModel modelData;			// モデル情報
		std::string sFilePassName;	// ファイルパス名
	};

	// メンバ関数
	HRESULT Init(void);		// モデル初期化
	void Uninit(void);		// モデル終了
	HRESULT LoadAll(void);	// モデル全読込
	int Regist(std::string sFilePass);	// モデル登録
	SModel *GetInfo(const int nID);		// モデル情報取得

	// 静的メンバ関数
	static CModel *Create(void);			// 生成
	static void Release(CModel *&pModel);	// 破棄

private:
	// メンバ関数
	HRESULT LoadXFileModel(SMapInfo *pMapInfo, std::string sFilePass);	// xファイルの読込
	HRESULT LoadTextureModel(SMapInfo *pMapInfo);		// テクスチャの読込
	HRESULT SetCollisionModel(SMapInfo *pMapInfo);		// 当たり判定の作成
	HRESULT SearchFolderAll(std::string sFolderPath);	// フォルダ全検索

	// メンバ変数
	std::map<int, SMapInfo> m_mapModel;	// モデル連想配列

	// 静的メンバ変数
	static int m_nNumAll;	// モデルの総数
};

#endif	// _MODEL_H_
