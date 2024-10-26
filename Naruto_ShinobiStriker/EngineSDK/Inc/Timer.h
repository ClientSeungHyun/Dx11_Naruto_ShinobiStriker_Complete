#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	float Get_TimeDelta() {	return m_fTimeDelta;	}

public:
	HRESULT Initialize();
	void Update();

public:
	void Start_Lack(_uint iLackCount);

private:
	LARGE_INTEGER			m_CurTime = {};
	LARGE_INTEGER			m_OldTime = {};
	LARGE_INTEGER			m_OriginTime = {};
	LARGE_INTEGER			m_CpuTick = {};

	/* �޼��� ������ �ѹ��� ���µ� �ɸ��� �ð�. */
	_float					m_fTimeDelta = { 0.f };

	_int					m_iLackCount = {};
	_bool					m_isLack = { false };

public:
	static CTimer* Create();
	virtual void Free() override;
};

END