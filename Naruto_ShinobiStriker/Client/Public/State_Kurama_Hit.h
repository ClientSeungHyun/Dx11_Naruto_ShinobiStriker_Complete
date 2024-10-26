#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_Hit :
    public CState
{
public:
    CState_Kurama_Hit(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_Hit() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Hit1 = {};
    _uint               m_iAnimation_Hit2 = {};

    _uint               m_iAnimation_ChanceHitLeft = {};
    _uint               m_iAnimation_ChanceHitRight = {};

    _uint               m_PrevStateIndex = { 0 };

private:
    void Look_Player();

public:
    static CState_Kurama_Hit* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END