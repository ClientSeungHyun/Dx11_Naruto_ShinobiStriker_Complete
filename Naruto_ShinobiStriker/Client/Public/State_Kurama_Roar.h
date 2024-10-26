#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Kurama_Roar :
    public CState
{
public:
    CState_Kurama_Roar(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Kurama_Roar() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Roar = {};
    _uint               m_iAnimation_Suggest = {};

    _uint               m_PrevStateIndex = {};

    _uint               m_iAttackFrame = { 32 };
    _uint               m_iAttackEndFrame = { 120 };

    _bool               m_isStartRoar = { false };
    _bool               m_isFrist = { true };
    _bool               m_isEndRoar = { false };
    _bool               m_isPlaySound = { false };

private:
    void Look_Player();

public:
    static CState_Kurama_Roar* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END