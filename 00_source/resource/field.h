//============================================================
//
//	�n�ʃw�b�_�[ [field.h]
//	Author�F���c�E��
//
//============================================================
//************************************************************
//	��d�C���N���[�h�h�~
//************************************************************
#ifndef _FIELD_H_
#define _FIELD_H_

//************************************************************
//	�C���N���[�h�t�@�C��
//************************************************************
#include "objectMeshField.h"

//************************************************************
//	�N���X��`
//************************************************************
// �n�ʃN���X
class CField : public CObjectMeshField
{
public:
	// �e�N�X�`����
	enum ETexture
	{
		TEXTURE_ASH = 0,	// �ΎR�D�e�N�X�`��
		TEXTURE_MAX			// ���̗񋓌^�̑���
	};

	// �n�`��
	enum ETerrain
	{
		TERRAIN_120x120 = 0,	// 120x120�������̒n�`
		TERRAIN_MAX				// ���̗񋓌^�̑���
	};

	// �R���X�g���N�^
	CField();

	// �f�X�g���N�^
	~CField() override;

	// �n�`���\����
	struct STerrainInfo
	{
		D3DXVECTOR3 *pPosGap;	// ���_���W�̂����
		POSGRID2 part;			// ������
	};

	// �I�[�o�[���C�h�֐�
	HRESULT Init(void) override;	// ������
	void Uninit(void) override;		// �I��
	void Update(const float fDeltaTime) override;	// �X�V
	void Draw(CShader *pShader = nullptr) override;	// �`��

	// �ÓI�����o�֐�
	static void LoadSetup(void);	// �Z�b�g�A�b�v
	static CField *Create	// ����
	( // ����
		const ETexture texture,		// ���
		const D3DXVECTOR3& rPos,	// �ʒu
		const D3DXVECTOR3& rRot,	// ����
		const D3DXVECTOR2& rSize,	// �傫��
		const D3DXCOLOR& rCol,		// �F
		const POSGRID2& rPart		// ������
	);

	// �����o�֐�
	void SetTerrain(const ETerrain terrain);	// �n�`�ݒ�

private:
	// �ÓI�����o�ϐ�
	static STerrainInfo m_aTerrainInfo[TERRAIN_MAX];	// �n�`���
};

#endif	// _FIELD_H_