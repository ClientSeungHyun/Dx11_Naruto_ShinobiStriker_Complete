#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CHead_Player final : public CPartObject
{
public:
	enum HEAD_ID { HEAD_HAIR1, HEAD_HAIR2, HEAD_CAP, HEAD_FROG, HEAD_HELMET, HEAD_HOKAGE, HEAD_HOOD, HEAD_PAJAMA, HEAD_END };

private:
	CHead_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHead_Player(const CHead_Player& Prototype);
	virtual ~CHead_Player() = default;

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
	HRESULT Change_Parts(_int iID, class CModel* pAnimModel = nullptr);

private:
	class CShader* m_pShaderCom = { nullptr };

	HEAD_ID m_eID = { HEAD_END };

	vector<class CModel*>	m_Models;
	_bool					m_isModel[HEAD_END];

	_float					m_fMotionTrailTime = {};
	class CPlayer_MotionTrail* m_pMotionTrail = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_MotionTrail();

public:
	static CHead_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END