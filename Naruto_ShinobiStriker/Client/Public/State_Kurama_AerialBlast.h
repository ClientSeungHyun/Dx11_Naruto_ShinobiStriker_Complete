#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_AerialBlast :
    public CState
{
public:
    CState_Kurama_AerialBlast(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_AerialBlast() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Blast = {};

    _bool               m_isShoot = { false };

    _float3             m_vTargetPos = {};

private:
    void Look_Player();

public:
    static CState_Kurama_AerialBlast* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END