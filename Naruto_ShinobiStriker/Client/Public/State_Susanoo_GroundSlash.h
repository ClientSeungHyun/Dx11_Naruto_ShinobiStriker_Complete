#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Susanoo_GroundSlash :
    public CState
{
public:
    CState_Susanoo_GroundSlash(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Susanoo_GroundSlash() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_GroundSlash = {};

    _uint               m_iAttackFrame = { 26 };

    _float              m_fDownTime = { 0.f };

    _bool               m_isAttack = { false };

private:



public:
    static CState_Susanoo_GroundSlash* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END