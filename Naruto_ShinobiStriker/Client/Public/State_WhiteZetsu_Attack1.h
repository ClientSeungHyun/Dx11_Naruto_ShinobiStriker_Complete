#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_WhiteZetsu_Attack1 :
    public CState
{
public:
    CState_WhiteZetsu_Attack1(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_WhiteZetsu_Attack1() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_DownStrike = {};
    _uint               m_iAnimation_JumpKick = {};

    _uint               m_iAttackFrameStart1 = { 18 };
    _uint               m_iAttackFrameEnd1 = { 22 };

    _uint               m_iAttackFrameStart2 = { 19 };
    _uint               m_iAttackFrameEnd2 = { 23 };

    _uint               m_iAttackFrameStart3 = { 31 };
    _uint               m_iAttackFrameEnd3 = { 35 };

    _float              m_AttackMoveSpped = { 5.f };

    _bool               m_isDownStrike = { false };

public:
    static CState_WhiteZetsu_Attack1* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END