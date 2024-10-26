#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Naruto_AerialRasengun_Run :
    public CState
{
public:
    CState_Naruto_AerialRasengun_Run(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Naruto_AerialRasengun_Run() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_AerialRasengun_RunStart = {};
    _uint               m_iAnimation_AerialRasengun_RunLoop = {};

    _bool               m_isRunStart = { false };

    _float              m_fRunTime = { 0.f };
    _float              m_fRunDuration = { 1.5f };

private:
    void                Chase_Target(_float fTimeDelta);

public:
    static CState_Naruto_AerialRasengun_Run* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END