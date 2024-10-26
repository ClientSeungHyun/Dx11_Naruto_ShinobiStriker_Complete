#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Dash :
    public CState
{
public:
    CState_Player_Dash(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Dash() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_DashLeft = {};
    _uint               m_iAnimation_DashRight = {};
    _uint               m_iAnimation_DashFront = {};
    _uint               m_iAnimation_DashBehind = {};

    _bool               m_isStop = { false };

private:
    void            Dash(_fvector vDashDir);
    void            Change_SkillState();

public:
    static CState_Player_Dash* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END