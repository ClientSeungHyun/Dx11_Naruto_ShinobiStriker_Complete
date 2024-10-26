#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_Chidori_Charge :
    public CState
{
public:
    CState_Custom_Chidori_Charge(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_Chidori_Charge() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Chidori_Charge1 = {};
    _uint               m_iAnimation_Chidori_Charge2to3 = {};
    _uint               m_iAnimation_Chidori_ChargeLoop = {};

    _bool               m_isCharge1 = { false };
    _bool               m_isCharge2to3 = { false };
    _bool               m_isChargeLoop = { false };

    _float              m_fAttackAmount = { 10.f };

public:
    static CState_Custom_Chidori_Charge* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END