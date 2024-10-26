#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Susanoo_DoubleSlash :
    public CState
{
public:
    CState_Susanoo_DoubleSlash(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Susanoo_DoubleSlash() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_DoubleSlash = {};

    _uint               m_iAttackFrame1 = { 8 };
    _uint               m_iAttackFrame2 = { 19 };

    _bool               m_isSlash1 = { false };
    _bool               m_isSlash2 = { false };

private:
    void Attack_Collider();
    void Look_Player();




public:
    static CState_Susanoo_DoubleSlash* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END