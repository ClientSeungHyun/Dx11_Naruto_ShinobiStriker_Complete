#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_JumpCrush :
    public CState
{
public:
    CState_Kurama_JumpCrush(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_JumpCrush() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_JumpCrush = {};

    _uint               m_iAttackFrame = { 83 };

    _bool               m_isAppearSmoke = { false };

private:
    void Attack_Collider();

public:
    static CState_Kurama_JumpCrush* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END