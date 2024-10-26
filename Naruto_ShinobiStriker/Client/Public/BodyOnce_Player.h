#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBodyOnce_Player final : public CPartObject
{
public:
	enum ONCE_ID { ONCE_HOKAGE, ONCE_ARMOR, ONCE_FINALCOAT, ONCE_JACKET, ONCE_SANTA, ONCE_SENNIN, ONCE_TRACK, ONCE_YUUKDO, ONCE_END };

private:
	CBodyOnce_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBodyOnce_Player(const CBodyOnce_Player& Prototype);
	virtual ~CBodyOnce_Player() = default;

public:
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

	virtual void On_MotionTrail() override;
	virtual void Off_Motiontrail() override;

public:
	HRESULT Change_Parts(_int iID);

private:
	class CShader*	m_pShaderCom = { nullptr };

	ONCE_ID			m_eID = { ONCE_END };

	vector<class CModel*>	m_Models;
	_bool					m_isModel[ONCE_END];

	_float					m_fMotionTrailTime = {};
	class CPlayer_MotionTrail* m_pMotionTrail = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_MotionTrail();

public:
	static CBodyOnce_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END