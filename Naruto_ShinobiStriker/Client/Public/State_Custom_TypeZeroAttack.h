#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_TypeZeroAttack :
    public CState
{
public:
    CState_Custom_TypeZeroAttack(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_TypeZeroAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };
    class CPlayerCamera* m_pPlayerCamera = { nullptr };

    _uint               m_iAnimation_TypeZero = {};
    _uint               m_iEffetFrames[10] = {};

    _float4x4           m_vInitWorldMatrix = {};

    _bool               m_isActiveEffects[10] = { false };
    _bool               m_isCameraControl = { false };

private:
    void                Control_Camera();
    void                Create_HitEffect();

public:
    static CState_Custom_TypeZeroAttack* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END