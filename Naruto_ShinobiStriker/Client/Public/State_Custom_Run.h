#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_Run :
    public CState
{
public:
    CState_Custom_Run(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_Run() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer*      m_pPlayer = { nullptr };

    _uint               m_iAnimation_RunLoop = {};

    _bool               m_isPlaySound = { false };
    _bool   m_ddd = false;

private:
    _bool               Move(_float fTimeDelta);
    void                Playe_Sound();
    _bool                Check_Ground(_float fTimeDelta);

public:
    static CState_Custom_Run* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END