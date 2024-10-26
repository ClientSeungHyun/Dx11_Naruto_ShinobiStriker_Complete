#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_ThreeBlast :
    public CState
{
public:
    CState_Kurama_ThreeBlast(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_ThreeBlast() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_ThreeBlast = {};

    _bool               m_isShoot[3] = {false};

    _float3             m_vTargetPos = {};

    _uint               m_iAttackFrame[3] = {};

private:
    void Look_Player();

public:
    static CState_Kurama_ThreeBlast* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END