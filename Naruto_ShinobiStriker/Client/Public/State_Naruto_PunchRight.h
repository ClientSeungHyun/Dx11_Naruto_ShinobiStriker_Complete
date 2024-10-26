#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Naruto_PunchRight :
    public CState
{
public:
    CState_Naruto_PunchRight(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Naruto_PunchRight() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_PunchRight = {};

    _bool               m_isChase = { false };

    _uint               m_iAttackFrame = { 11 };

private:
    void                Attack_Collider();

public:
    static CState_Naruto_PunchRight* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END