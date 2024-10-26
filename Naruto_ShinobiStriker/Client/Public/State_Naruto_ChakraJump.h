#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Naruto_ChakraJump :
    public CState
{
public:
    CState_Naruto_ChakraJump(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Naruto_ChakraJump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_ChakraJump_Start = {};
    _uint               m_iAnimation_ChakraJump_Loop = {};
    _uint               m_iAnimation_ChakraJump_End = {};

    _bool               m_isStart = { false };
    _bool               m_isLoop = { false };
    _bool               m_isEnd = { false };

    _float              m_fJumpTime = { 0.f };
    _float              m_fSpeed = { 80.f };
    _float3             m_vStartPos = {};
    _float3             m_vJumpDir = {};

    _float              m_fJumpPower = { 0.f };

private:
private:
    void                Set_JumpDir();
    void                ChakraJump();

public:
    static CState_Naruto_ChakraJump* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END