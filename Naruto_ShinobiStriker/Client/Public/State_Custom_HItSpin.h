#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_HitSpin :
    public CState
{
public:
    CState_Custom_HitSpin(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_HitSpin() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer*      m_pPlayer = { nullptr };

    _uint               m_iAnimation_Spin = {};
    _uint               m_iAnimation_FallBehind = {};
    _uint               m_iAnimation_DownFloor = {};
    _uint               m_iAnimation_GetUpReady = {};
    _uint               m_iAnimation_GetUp = {};

    _float              m_fCurrentTime = { 0.f };
    _float              m_fDownDuration = { 2.f };

    _bool               m_isFallEnd = { false };

public:
    static CState_Custom_HitSpin* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END