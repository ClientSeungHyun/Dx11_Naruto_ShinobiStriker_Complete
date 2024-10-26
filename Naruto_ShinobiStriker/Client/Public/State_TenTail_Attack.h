#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_TenTail_Attack :
    public CState
{
public:
    CState_TenTail_Attack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_TenTail_Attack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Punch = {};
    _uint               m_iAnimation_DownStrike = {};

    _uint               m_iAttackFrameStart1 = { 64 };
    _uint               m_iAttackFrameEnd1 = { 70 };

    _uint               m_iAttackFrameStart2 = { 91 };
    _uint               m_iAttackFrameEnd2 = { 95 };

    _float              m_AttackMoveSpped = { 5.f };

    _bool               m_isPunch = { false };

public:
    static CState_TenTail_Attack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END