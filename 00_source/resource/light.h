//============================================================
//
//	���C�g�w�b�_�[ [light.h]
//	Author�F���c�E��
//
//============================================================
//************************************************************
//	��d�C���N���[�h�h�~
//************************************************************
#ifndef _LIGHT_H_
#define _LIGHT_H_

//************************************************************
//	�萔�錾
//************************************************************
namespace light
{
	const int MAX_NUM = 4;	// �g�p���C�g��
}

//************************************************************
//	�N���X��`
//************************************************************
// ���C�g�N���X
class CLight
{
public:
	// �R���X�g���N�^
	CLight();

	// �f�X�g���N�^
	~CLight();

	// �����o�֐�
	HRESULT Init(void);	// ������
	void Uninit(void);	// �I��
	void Update(const float fDeltaTime);	// �X�V
	D3DLIGHT9 GetLight(const int nID);		// ���C�g�擾

	// �ÓI�����o�֐�
	static CLight *Create(void);			// ����
	static void Release(CLight *&pLight);	// �j��

private:
	// �����o�ϐ�
	D3DLIGHT9 m_aLight[light::MAX_NUM];	// ���C�g�̏��
};

#endif	// _LIGHT_H_