#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBodyLower_Player final : public CPartObject
{
public:
	enum LOWER_ID { LOWER_PANTS1, LOWER_PANTS2, LOWER_ANBU, LOWER_ARMOR, LOWER_CUT, LOWER_SHORT, LOWER_SKIRT, LOWER_VEST, LOWER_END };

private:
	CBodyLower_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBodyLower_Player(const CBodyLower_Player& Prototype);
	virtual ~CBodyLower_Player() = default;

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
	HRESULT Change_Parts(_int eID, class CModel* pAnimModel = nullptr);

private:
	class CShader* m_pShaderCom = { nullptr };

	LOWER_ID m_eID = LOWER_END;

	vector<class CModel*>	m_Models;
	_bool					m_isModel[LOWER_END];

private:
	HRESULT Ready_Components();

public:
	static CBodyLower_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END