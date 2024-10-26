#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_WireDash :
    public CState
{
public:
    CState_Custom_WireDash(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_WireDash() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_AerialDash_Start = {};
    _uint               m_iAnimation_AerialDash_Loop = {};
    _uint               m_iAnimation_AerialDash_End = {};

    _uint               m_iWireFrame = { 10 };

    _float3             m_vTargetDir = {};

public:
    static CState_Custom_WireDash* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END