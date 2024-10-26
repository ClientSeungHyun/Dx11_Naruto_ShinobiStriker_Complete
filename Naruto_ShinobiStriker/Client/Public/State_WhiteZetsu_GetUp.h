#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_WhiteZetsu_GetUp :
    public CState
{
public:
    CState_WhiteZetsu_GetUp(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_WhiteZetsu_GetUp() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_GetUp = {};

public:
    static CState_WhiteZetsu_GetUp* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum);
    virtual void Free() override;

};

END