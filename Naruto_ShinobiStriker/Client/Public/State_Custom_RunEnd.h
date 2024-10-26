#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_RunEnd :
    public CState
{
public:
    CState_Custom_RunEnd(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_RunEnd() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_RunnEnd = {};

private:
    _bool               Move(_float fTimeDelta);

public:
    static CState_Custom_RunEnd* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END