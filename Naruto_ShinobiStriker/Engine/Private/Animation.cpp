#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const CModel* pModel)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(*pFile, &m_SpeedPerSec, sizeof(_double), &dwByte, nullptr);

	/* �� �ִϸ��̼��� ����ϴ� ���� ������ �����Ѵ�. */
	ReadFile(*pFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	KeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pFile, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

_uint CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _bool* isEnd, _float fTimeDelta)
{
	/* ���� �����ġ�� ����ϳ�. */
	*pCurrentTrackPosition += m_SpeedPerSec * m_SpeedRatio * fTimeDelta;

	m_iCurrentFrame = 0;
	if (*pCurrentTrackPosition >= m_Duration)
	{
		if (false == isLoop)
		{
			*isEnd = true;
			return 0;
		}

		*pCurrentTrackPosition = 0.f;
	}

	/* ���� �����ġ�� �°� ���� �ִϸ��̼��� ��Ʈ���ؾ� �� ���� ���µ��� �������ش�. */
	_uint	iChannelIndex = { 0 };
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, &CurrentKeyFrameIndices[iChannelIndex], *pCurrentTrackPosition);
		if (m_iCurrentFrame < CurrentKeyFrameIndices[iChannelIndex])
			m_iCurrentFrame = CurrentKeyFrameIndices[iChannelIndex];

		iChannelIndex++;
	}

	return m_iCurrentFrame;
}

void CAnimation::Reset()
{
	m_iCurrentFrame = 0;
}

void CAnimation::Research_BoneIndex(const CModel* pModel)
{
	for (auto& Channel : m_Channels)
	{
		Channel->Research_BoneIndex(pModel);
	}
}

CAnimation* CAnimation::Create(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pFile, KeyFrameIndices, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}
