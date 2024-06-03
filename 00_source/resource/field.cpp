//============================================================
//
//	地面処理 [field.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "field.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャファイル
	{
		"data\\TEXTURE\\field000.png",	// 火山灰テクスチャ
	};

	const char *SETUP_TXT = "data\\TXT\\field.txt";	// セットアップテキスト相対パス

	const int PRIORITY = 0;	// 地面の優先順位
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CField::STerrainInfo CField::m_aTerrainInfo[TERRAIN_MAX] = {};	// 地形情報

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CField::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CField] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CField::CField() : CObjectMeshField(CObject::LABEL_FIELD, CObject::DIM_3D, PRIORITY)
{

}

//============================================================
//	デストラクタ
//============================================================
CField::~CField()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CField::Init(void)
{
	// オブジェクトメッシュフィールドの初期化
	if (FAILED(CObjectMeshField::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// セットアップの読込
	LoadSetup();

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CField::Uninit(void)
{
	for (int nCntField = 0; nCntField < TERRAIN_MAX; nCntField++)
	{ // 地形情報の最大数分繰り返す

		// 地形情報の破棄
		SAFE_DEL_ARRAY(m_aTerrainInfo[nCntField].pPosGap);
	}

	// オブジェクトメッシュフィールドの終了
	CObjectMeshField::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CField::Update(const float fDeltaTime)
{
	// オブジェクトメッシュフィールドの更新
	CObjectMeshField::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CField::Draw(CShader *pShader)
{
	// オブジェクトメッシュフィールドの描画
	CObjectMeshField::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CField *CField::Create
(
	const ETexture texture,		// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR2& rSize,	// 大きさ
	const D3DXCOLOR& rCol,		// 色
	const POSGRID2& rPart		// 分割数
)
{
	// 地面の生成
	CField *pField = new CField;
	if (pField == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 地面の初期化
		if (FAILED(pField->Init()))
		{ // 初期化に失敗した場合

			// 地面の破棄
			SAFE_DELETE(pField);
			return nullptr;
		}

		// テクスチャを登録・割当
		pField->BindTexture(GET_MANAGER->GetTexture()->Regist(TEXTURE_FILE[texture]));

		// 位置を設定
		pField->SetVec3Position(rPos);

		// 向きを設定
		pField->SetVec3Rotation(rRot);

		// 大きさを設定
		pField->SetVec2Sizing(rSize);

		// 色を設定
		pField->SetColor(rCol);

		// 分割数を設定
		if (FAILED(pField->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// 地面の破棄
			SAFE_DELETE(pField);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pField;
	}
}

//============================================================
//	地形の設定処理
//============================================================
void CField::SetTerrain(const ETerrain terrain)
{
	if (terrain > NONE_IDX && terrain < TERRAIN_MAX)
	{ // 値が範囲内の場合

		// 地形を設定
		CObjectMeshField::SetTerrain(m_aTerrainInfo[terrain].part, m_aTerrainInfo[terrain].pPosGap);
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	セットアップ処理
//============================================================
void CField::LoadSetup(void)
{
	// 変数を宣言
	int nID = 0;		// インデックスの代入用
	int nNumVtx = 0;	// 頂点数の代入用
	int nEnd = 0;		// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	for (int nCntField = 0; nCntField < TERRAIN_MAX; nCntField++)
	{ // 地形情報の最大数分繰り返す

		// 地形情報の破棄
		SAFE_DEL_ARRAY(m_aTerrainInfo[nCntField].pPosGap);
	}

	// 静的メンバ変数の情報をクリア
	memset(&m_aTerrainInfo[0], 0, sizeof(m_aTerrainInfo));	// 地形情報

	// ファイルを読み込み形式で開く
	pFile = fopen(SETUP_TXT, "r");

	if (pFile != nullptr)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// 地形の設定
			if (strcmp(&aString[0], "TERRAINSET") == 0)
			{ // 読み込んだ文字列が TERRAINSET の場合

				do
				{ // 読み込んだ文字列が END_TERRAINSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "FIELDSET") == 0)
					{ // 読み込んだ文字列が FIELDSET の場合

						do
						{ // 読み込んだ文字列が END_FIELDSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "PART") == 0)
							{ // 読み込んだ文字列が PART の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aTerrainInfo[nID].part.x);	// 分割数Xを読み込む
								fscanf(pFile, "%d", &m_aTerrainInfo[nID].part.y);	// 分割数Yを読み込む

								// 頂点数を設定
								nNumVtx = (m_aTerrainInfo[nID].part.x + 1) * (m_aTerrainInfo[nID].part.y + 1);

								if (m_aTerrainInfo[nID].pPosGap == nullptr)
								{ // ポインタが使用されていない場合

									// 頂点数分メモリ確保
									m_aTerrainInfo[nID].pPosGap = new D3DXVECTOR3[nNumVtx];

									// 例外処理
									assert(m_aTerrainInfo[nID].pPosGap != nullptr);	// 非使用中
								}
								else { assert(false); }	// 使用中
							}
							else if (strcmp(&aString[0], "GAPSET") == 0)
							{ // 読み込んだ文字列が GAPSET の場合

								for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
								{ // 頂点数分繰り返す

									fscanf(pFile, "%f", &m_aTerrainInfo[nID].pPosGap[nCntVtx].x);	// 頂点座標のずれ量Xを読み込む
									fscanf(pFile, "%f", &m_aTerrainInfo[nID].pPosGap[nCntVtx].y);	// 頂点座標のずれ量Yを読み込む
									fscanf(pFile, "%f", &m_aTerrainInfo[nID].pPosGap[nCntVtx].z);	// 頂点座標のずれ量Zを読み込む
									fscanf(pFile, "%s", &aString[0]);								// , を読み込む (不要)
								}
							}
						} while (strcmp(&aString[0], "END_FIELDSET") != 0);	// 読み込んだ文字列が END_FIELDSET ではない場合ループ

						// インデックスを加算
						nID++;
					}
				} while (strcmp(&aString[0], "END_TERRAINSET") != 0);	// 読み込んだ文字列が END_TERRAINSET ではない場合ループ
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(nullptr, "地面セットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
