#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_Rasenko :
    public CState
{
public:
    CState_Custom_Rasenko(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_Rasenko() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };
    class CPlayerCamera* m_pPlayerCamera = { nullptr };

    _uint               m_iAnimation_Rasenko_Start = {};
    _uint               m_iAnimation_Rasenko_Loop = {};
    _uint               m_iAnimation_Rasenko_End = {};

    _float              m_fLoopTime = {};
    _float              m_fCurrentTime = { };

    _bool               m_isCreateRasengan = { false };
    _bool               m_isCreateStar = { false };
    _bool               m_isMoveCenter = { false };
    _bool               m_isStartCamera = { false };
    _bool               m_isSecondCamera = { false };
    _bool               m_isShootKunai = { false };
    _bool               m_isAppearKunai = { false };

private:
    void                Control_Camera();

public:
    static CState_Custom_Rasenko* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END