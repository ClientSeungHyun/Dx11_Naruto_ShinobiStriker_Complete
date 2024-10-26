#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_Attack2 :
    public CState
{
public:
    CState_Kurama_Attack2(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_Attack2() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_DownScratch = {};
    _uint               m_iAnimation_UpScratch = {};
    _uint               m_iAnimation_TurnKick = {};

    _uint               m_iAttackFrame1 = { 58 };
    _uint               m_iAttackFrame2 = { 40 };
    _uint               m_iAttackFrame3 = { 20 };

private:
    void Look_Player();
    void Attack_Collider();

public:
    static CState_Kurama_Attack2* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END