#pragma once

#include "Base.h"

BEGIN(Engine)

/* �ִϸ��̼� ������ ���� ����ϴ� �� �� �ϳ� */
/* �ð��� ���� ���� ������ ������ �ִ�. �ʱ� ������ ��� �ð��� �ٸ� ���°����� �̸� �����صд�. */
/* ���� ��Ÿ�Ӱ��� �� ä�ο��� ���� �ִϸ��̼��� ��� ��ġ�� �������ָ� �����ġ�� �´� �������(m_TransformationMatrix)�� �����. */

/* KeyFrame : Ư�� �����ġ���� �� ���� ���ؾ��� ����(���)�� ����(������)�Ͽ� ǥ���� ����ü.  */

class CChannel : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(HANDLE* pFile, const aiNodeAnim* pAIChannel);

private:
	_char				m_szName[MAX_PATH] = "";

	_uint				m_iNumKeyFrames = {};
	vector<KEYFRAME>	m_KeyFrames;

public:
	static CChannel* Create(HANDLE* pFile, const aiNodeAnim* pAIChannel);
	virtual void Free() override;

};

END