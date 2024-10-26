#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_WhiteZetsu_Idle :
    public CState
{
public:
    CState_WhiteZetsu_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_WhiteZetsu_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster*     m_pMonster = { nullptr };

    _uint               m_iAnimation_Idle = {};

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleDuration{ 0.5f };

    _float              m_fHitTime = { 0.f };

public:
    static CState_WhiteZetsu_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END