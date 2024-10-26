#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Susanoo_ChanceTime :
    public CState
{
public:
    CState_Susanoo_ChanceTime(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Susanoo_ChanceTime() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_ChanceStart = {};
    _uint               m_iAnimation_ChanceLoop = {};
    _uint               m_iAnimation_ChanceEnd = {};

    _float              m_fChanceTime = { 0.f };
    _float              m_fChaneDuration = { 4.f };

    _bool               m_isEnd = { false };

private:
    void Look_Player();

public:
    static CState_Susanoo_ChanceTime* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END