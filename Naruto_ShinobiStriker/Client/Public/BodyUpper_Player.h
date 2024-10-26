#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBodyUpper_Player final : public CPartObject
{
public:
	enum UPPER_ID { UPPER_TRAINER1, UPPER_TRAINER2, UPPER_ANBU, UPPER_ARMOR, UPPER_HAORI, UPPER_HOODIE, UPPER_SHIRT, UPPER_VEST, UPPER_END };

private:
	CBodyUpper_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBodyUpper_Player(const CBodyUpper_Player& Prototype);
	virtual ~CBodyUpper_Player() = default;

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

public:
	HRESULT Change_Parts(_int iID, class CModel* pAnimModel = nullptr);

private:
	class CShader* m_pShaderCom = { nullptr };
	
	UPPER_ID m_eID = UPPER_END;

	vector<class CModel*>	m_Models;
	_bool					m_isModel[UPPER_END];

private:
	HRESULT Ready_Components();

public:
	static CBodyUpper_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END