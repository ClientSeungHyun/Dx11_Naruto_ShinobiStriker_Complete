#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	_char*						Get_Name() { return m_szName; }

	vector<class CChannel*>&	Get_Channels() { return m_Channels; }
	void						Set_Channels(vector<class CChannel*>& Channels) { m_Channels = Channels; }

	_double						Get_SpeedRatio() { return m_SpeedRatio; }
	void						Set_SpeedRatio(_double SpeedRatio) { m_SpeedRatio = SpeedRatio; }

public:
	HRESULT Initialize(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const class CModel* pModel);
	_uint Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _bool* isEnd, _float fTimeDelta);


public:
	void			Reset();
	void			Research_BoneIndex(const CModel* pModel);

private:
	_char						m_szName[MAX_PATH] = {};

	/* �� �ִϸ��̼��� ����ϴ� ���� ���� */
	_uint						m_iNumChannels = { 0 };

	/* ���� ������ ���� */
	/* Channel : �� (�ð��� ���� �� ���� ���� ����� �����Ѵ�) */
	vector<class CChannel*>		m_Channels;

	/* �� �ִϸ��̼��� �����ϱ����� �ɸ��� ��ü �Ÿ�. */
	_double						m_Duration = { 0.0 };

	/* �ִϸ��̼��� �ʴ� ����ӵ� */
	_double						m_SpeedPerSec = { 0.0 };
	_double						m_SpeedRatio = { 0.0 };

	_uint						m_iCurrentFrame = {};

public:
	static CAnimation* Create(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const class CModel* pModel);
	virtual void Free() override;
};

END