#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_MassiveRasengan :
    public CState
{
public:
    CState_Custom_MassiveRasengan(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_MassiveRasengan() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };
    class CPlayerCamera* m_pPlayerCamera = { nullptr };

    _uint               m_iAnimation_MassiveRasengan = {};

    _uint               m_iFrame_Create = { 17 };
    _uint               m_iFrame_Shoot = { 63 };

    _uint               m_iCameraFrame1 = { 21 };

    _bool               m_isCameraMove1 = { false };
    _bool               m_isCameraMove2 = { false };

    _bool               m_isShoot = { false };

    _float              m_fMoveTime = {};

    _float3             m_vInitPos = {};
    _float3             m_vTargetPos = {};

private:
    void                Control_Camera(_float fTimeDelta);
    void                Chase_Target(_float fTimeDelta);
    void                Setting_TargetPos();
    void                Create_Shadow();

public:
    static CState_Custom_MassiveRasengan* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END