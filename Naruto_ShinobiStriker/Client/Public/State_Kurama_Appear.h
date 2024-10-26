#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_Appear :
    public CState
{
public:
    CState_Kurama_Appear(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_Appear() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Appear = {};

    _bool               m_isFirst = { true };
    _bool               m_isPlayLandSound = { false };

private:
    void Look_Player();

public:
    static CState_Kurama_Appear* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END