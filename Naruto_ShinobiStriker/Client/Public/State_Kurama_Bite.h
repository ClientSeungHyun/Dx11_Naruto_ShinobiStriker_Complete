#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_Bite :
    public CState
{
public:
    CState_Kurama_Bite(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_Bite() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Bite = {};

    _uint               m_iAttackFrame = { 40 };
    
private:
    void Attack_Collider();



public:
    static CState_Kurama_Bite* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END