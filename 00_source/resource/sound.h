//============================================================
//
//	サウンドヘッダー [sound.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

//************************************************************
//	クラス定義
//************************************************************
// サウンドクラス
class CSound
{
public:
	// コンストラクタ
	CSound();

	// デストラクタ
	~CSound();

	// サウンド列挙
	enum ELabel
	{
		LABEL_BGM_GENERAL = 0,		// BGM (汎用)
		LABEL_BGM_TUTORIAL,			// BGM (チュートリアル)
		LABEL_BGM_GAME,				// BGM (ゲーム)

		LABEL_SE_SELECT_000,		// 選択操作音00
		LABEL_SE_SELECT_001,		// 選択操作音01
		LABEL_SE_DECISION_000,		// 決定音00
		LABEL_SE_DECISION_001,		// 決定音01
		LABEL_SE_DECISION_002,		// 決定音02
		LABEL_SE_SWORD_SWING_000,	// 剣の風切り音00
		LABEL_SE_SWORD_SWING_001,	// 剣の風切り音01
		LABEL_SE_SWORD_SWING_002,	// 剣の風切り音02
		LABEL_SE_PLAYER_HIT,		// プレイヤーのヒット音
		LABEL_SE_SWORD_HIT,			// 剣のヒット音
		LABEL_SE_PLAYER_DEATH,		// プレイヤーの死亡音
		LABEL_SE_SWORD_APPEAL,		// 剣のアピール音
		LABEL_SE_LAST_ATTACK,		// 最終攻撃音
		LABEL_SE_WALK,				// 足音
		LABEL_SE_LAND_S,			// 着地音 (小)
		LABEL_SE_LAND_B,			// 着地音 (大)
		LABEL_SE_PLAYER_ACTION,		// プレイヤーのアクション音
		LABEL_SE_DRAGON_ROAR_L,		// ドラゴン咆哮 (長)
		LABEL_SE_DRAGON_ROAR_S,		// ドラゴン咆哮 (短)
		LABEL_SE_WING,				// 羽ばたき音
		LABEL_SE_TELEPORT,			// テレポート音
		LABEL_SE_PUNCH,				// パンチ音
		LABEL_SE_CLAW,				// ひっかき音
		LABEL_SE_TAIL,				// しっぽ音
		LABEL_SE_NOISE,				// ノイズ音
		LABEL_SE_EXPLOSION,			// 爆発音
		LABEL_SE_FIRE,				// 炎音
		LABEL_SE_THUNDER,			// 雷音
		LABEL_SE_THUNDER_WARN,		// 雷警告音
		LABEL_MAX					// この列挙型の総数
	};

	// サウンド情報構造体
	struct SSoundInfo
	{
		const char *pFilename;	// ファイル名
		int nCntLoop;			// ループカウント
	};

	// メンバ関数
	HRESULT Init(HWND hWnd);	// 初期化
	void Uninit(void);			// 終了
	HRESULT LoadAll(HWND hWnd);	// 全読込
	HRESULT Play(ELabel label);	// 再生
	void Stop(ELabel label);	// 停止
	void Stop(void);			// 全停止
	void SetVolume(const ELabel label, float fVolume);	// 音量設定
	float GetVolume(const ELabel label) const;			// 音量取得
	void SetFrequency(const ELabel label, float fFreq);	// 周波数設定
	float GetFrequency(const ELabel label) const;		// 周波数取得

	// 静的メンバ関数
	static CSound *Create(HWND hWnd);		// 生成
	static void Release(CSound *&pSound);	// 破棄

private:
	// メンバ関数
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);	// チャンクチェック
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);	// チャンクデータ読込

	// メンバ変数
	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス
	IXAudio2SourceVoice *m_apSourceVoice[LABEL_MAX];	// ソースボイス
	BYTE *m_apDataAudio[LABEL_MAX];						// オーディオデータ
	DWORD m_aSizeAudio[LABEL_MAX];						// オーディオデータサイズ
};

#endif	// _SOUND_H_
