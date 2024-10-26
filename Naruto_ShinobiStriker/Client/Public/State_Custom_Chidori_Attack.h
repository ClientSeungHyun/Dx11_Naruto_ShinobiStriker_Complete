#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_Chidori_Attack :
    public CState
{
public:
    CState_Custom_Chidori_Attack(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_Chidori_Attack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Chidori_Attack = {};

public:
    static CState_Custom_Chidori_Attack* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END