#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_Heal :
    public CState
{
public:
    CState_Kurama_Heal(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_Heal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_HealStart = {};
    _uint               m_iAnimation_HealLoop = {};
    _uint               m_iAnimation_HealEnd = {};

    _bool               m_isHealEnd = {};

private:
    void Look_Player();

public:
    static CState_Kurama_Heal* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END