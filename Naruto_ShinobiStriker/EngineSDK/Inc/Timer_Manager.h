#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	HRESULT Add_Timer(const _wstring& strTimerTag);	
	_float Compute_TimeDelta(const _wstring& strTimerTag);

public:
	_float Get_TimeDelta(const _wstring& strTimerTag);
	void Start_Lack(const _wstring& strTimerTag, _uint iLackCount);

private:
	map<const _wstring, class CTimer*>			m_Timers;

private:
	class CTimer* Find_Timer(const _wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free() override;
};

END