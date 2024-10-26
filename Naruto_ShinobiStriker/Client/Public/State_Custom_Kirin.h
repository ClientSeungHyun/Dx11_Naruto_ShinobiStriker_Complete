#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Custom_Kirin :
    public CState
{
public:
    CState_Custom_Kirin(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Custom_Kirin() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };
    class CPlayerCamera* m_pPlayerCamera = { nullptr };

    _uint               m_iAnimation_Kirin = {};

    _uint               m_iCameraFrame1 = { 30 };
    _uint               m_iCameraFrame2 = { 45 };
    _uint               m_iShootFrame = { 60 };

    _float3             m_vKirinPos = {};

    _float3             m_vCameraStartPos1 = {};
    _float3             m_vCameraEndPos1 = {};
    _bool               m_isCameraMove1 = { false };

    _float3             m_vCameraPos2 = {};
    _bool               m_isCameraMove2 = { false };

    _float3             m_vCameraAt1 = {};
    _float3             m_vCameraAt2 = {};

    _bool               m_isCreate = { false };

public:
    static CState_Custom_Kirin* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;

};

END