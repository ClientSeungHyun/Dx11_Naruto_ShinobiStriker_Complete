#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_Attack1 :
    public CState
{
public:
    CState_Kurama_Attack1(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_Attack1() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster*     m_pMonster = { nullptr };

    _uint               m_iAnimation_DownRight = {};
    _uint               m_iAnimation_DownLeft = {};
    _uint               m_iAnimation_KickUp = {};

    _uint               m_iAttackFrame1 = { 40 };
    _uint               m_iAttackFrame2 = { 33 };
    _uint               m_iAttackFrame3 = { 27 };

private:
    void Look_Player();
    void Attack_Collider();

public:
    static CState_Kurama_Attack1* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END