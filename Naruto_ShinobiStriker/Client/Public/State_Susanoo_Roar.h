#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Susanoo_Roar :
    public CState
{
public:
    CState_Susanoo_Roar(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Susanoo_Roar() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Roar = {};

    _bool               m_isSummonBalde = { false };
    _bool               m_isPlaySound = { false };

public:
    static CState_Susanoo_Roar* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END