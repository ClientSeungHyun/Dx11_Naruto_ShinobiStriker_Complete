#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(HANDLE* pFile, const aiAnimation* pAIAnimation);

private:
	_char						m_szName[MAX_PATH] = {};

	/* �� �ִϸ��̼��� ����ϴ� ���� ���� */
	_uint						m_iNumChannels = { 0 };

	/* ���� ������ ���� */
	/* Channel : �� (�ð��� ���� �� ���� ���� ����� �����Ѵ�) */
	vector<class CChannel*>		m_Channels;

	/* �� �ִϸ��̼��� �����ϱ����� �ɸ��� ��ü �Ÿ�. */
	_double						m_Duration = {};

	/* �ִϸ��̼��� �ʴ� ����ӵ� */
	_double						m_SpeedPerSec = {};

	_double						m_CurrentTrackPosition = {};

public:
	static CAnimation* Create(HANDLE* pFile, const aiAnimation* pAIAnimation);
	virtual void Free() override;
};

END