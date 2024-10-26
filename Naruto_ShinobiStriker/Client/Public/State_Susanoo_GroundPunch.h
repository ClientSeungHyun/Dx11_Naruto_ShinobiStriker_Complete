#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Susanoo_GroundPunch :
    public CState
{
public:
    CState_Susanoo_GroundPunch(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Susanoo_GroundPunch() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_GroundPunch = {};

    _uint               m_iAttackFrame = { 18 };

    _bool               m_isSummonGroundBreak = { false };

private:
    void Attack_Collider();
    void Look_Player();



public:
    static CState_Susanoo_GroundPunch* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END