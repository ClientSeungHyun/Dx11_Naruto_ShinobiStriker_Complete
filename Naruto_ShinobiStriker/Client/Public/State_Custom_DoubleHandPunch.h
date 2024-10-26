#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_DoubleHandPunch :
    public CState
{
public:
    CState_Custom_DoubleHandPunch(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_DoubleHandPunch() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_DoublePunch = {};

    _bool               m_isChase = { false };
    _bool               m_isSound = { false };

    _uint               m_iAttackFrame = { 14 };

private:
    void                Attack_Collider();

public:
    static CState_Custom_DoubleHandPunch* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END