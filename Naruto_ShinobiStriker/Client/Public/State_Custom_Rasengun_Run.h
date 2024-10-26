#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_Rasengun_Run :
    public CState
{
public:
    CState_Custom_Rasengun_Run(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_Rasengun_Run() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Rasengun_RunStart = {};
    _uint               m_iAnimation_Rasengun_RunLoop = {};

    _bool               m_isRunStart = { false };

    _float              m_fRunTime = { 0.f };
    _float              m_fRunDuration = { 1.2f };

private:
    void                Chase_Target(_float fTimeDelta);

public:
    static CState_Custom_Rasengun_Run* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END