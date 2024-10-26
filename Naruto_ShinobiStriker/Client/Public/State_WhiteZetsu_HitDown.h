#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_WhiteZetsu_HitDown :
    public CState
{
public:
    CState_WhiteZetsu_HitDown(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_WhiteZetsu_HitDown() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_HitDown = {};

public:
    static CState_WhiteZetsu_HitDown* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END