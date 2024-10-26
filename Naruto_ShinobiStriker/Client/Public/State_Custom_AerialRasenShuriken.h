#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_AerialRasenShuriken :
    public CState
{
public:
    CState_Custom_AerialRasenShuriken(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_AerialRasenShuriken() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_AerialRasenShuriken = {};

    _uint               m_iFrame_Create = { 10 };
    _uint               m_iFrame_Shoot = { 70 };

    _bool               m_isShoot = { false };


private:
    void                Chase_Target(_float fTimeDelta);

public:
    static CState_Custom_AerialRasenShuriken* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END