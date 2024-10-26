#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_WhiteZetsu_AttackFar :
    public CState
{
public:
    CState_WhiteZetsu_AttackFar(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_WhiteZetsu_AttackFar() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_GroundPunch = {};

    _uint               m_iAttackFrameStart = { 82 };
    _uint               m_iAttackFrameEnd = { 85 };

public:
    static CState_WhiteZetsu_AttackFar* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END