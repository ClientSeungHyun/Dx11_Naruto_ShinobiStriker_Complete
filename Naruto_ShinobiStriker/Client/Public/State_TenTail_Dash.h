#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_TenTail_Dash :
    public CState
{
public:
    CState_TenTail_Dash(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_TenTail_Dash() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Dash = {};

    _float3             m_vDashDir = {};

public:
    static CState_TenTail_Dash* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END