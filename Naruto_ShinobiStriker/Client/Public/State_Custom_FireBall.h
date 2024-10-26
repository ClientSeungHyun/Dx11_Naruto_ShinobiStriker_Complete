#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_FireBall :
    public CState
{
public:
    CState_Custom_FireBall(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_FireBall() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_FireBall = {};

    _bool               m_isCreate = { false };

public:
    static CState_Custom_FireBall* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END