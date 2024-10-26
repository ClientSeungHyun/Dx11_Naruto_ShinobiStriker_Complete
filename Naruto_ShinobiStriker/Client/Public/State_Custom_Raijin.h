#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_Raijin :
    public CState
{
public:
    CState_Custom_Raijin(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_Raijin() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };
    class CPlayerCamera* m_pPlayerCamera = { nullptr };

    _uint               m_iAnimation_RaijinStart = {};
    _uint               m_iAnimation_RaijinHit = {};
    _uint               m_iAnimation_RaijinEnd = {};

    _float3             m_vTargetPos = { };

    _bool               m_isCameraControl = { false };
    _bool               m_isCreateKunai = { false };
    _bool               m_isMoveTarget = { false };
    _bool               m_isBackJump = { false };

private:
    void                Control_Camera();
    void                Create_HitEffect();

    void                Setting_TargetPos();

public:
    static CState_Custom_Raijin* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END