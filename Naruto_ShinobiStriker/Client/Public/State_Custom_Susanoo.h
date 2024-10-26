#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_Susanoo :
    public CState
{
public:
    CState_Custom_Susanoo(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_Susanoo() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };
    class CSkill_Susanoo* m_pSusanoo = { nullptr };

    class CPlayerCamera* m_pPlayerCamera = { nullptr };

    _uint               m_iAnimation_SusanooStart = {};
    _uint               m_iAnimation_SusanooLoop = {};
    _uint               m_iAnimation_SusanooEnd = {};

    _float              m_fLoopTime = {};
    _float              m_fLoopDuration = {};
    _float3             m_vTargetPos = { };

    _bool               m_isCameraControl = { false };
    _bool               m_isCreateSusanoo = { false };
    _bool               m_isPlayerRender = { false };

private:
    void                Control_Camera();
    void                Create_HitEffect();

    void                Setting_TargetPos();

public:
    static CState_Custom_Susanoo* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END