#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CFace_Player final : public CPartObject
{
public:
	enum FACE_ID { FACE_BASE, FACE_BAND, FACE_MASK1, FACE_MASK2, FACE_END };
	enum EYE_ID { EYE_BASE, EYE_SYARIN, EYE_BYAKU, EYE_SENNIN, EYE_NINETAIL, EYE_MANGITCHI, EYE_MANGSASUKE, EYE_RINNEN, EYE_END};

private:
	CFace_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFace_Player(const CFace_Player& Prototype);
	virtual ~CFace_Player() = default;

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
	HRESULT Change_Eye(_int iID);

private:
	class CShader* m_pShaderCom = { nullptr };

	FACE_ID m_eID = { FACE_END };

	_uint					m_iEyeMaterialIndex = { 0 };
	_uint					m_iEyeIndex = { 0 };

	vector<class CModel*>	m_Models;
	_bool					m_isModel[FACE_END];

	_float					m_fMotionTrailTime = {};
	class CPlayer_MotionTrail* m_pMotionTrail = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_MotionTrail();

	void Add_EyeTexture(class CModel* pModel);

public:
	static CFace_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END