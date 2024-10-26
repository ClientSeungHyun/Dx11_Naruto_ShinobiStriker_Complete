#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CJumpGuide final : public CGameObject
{
private:
	CJumpGuide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJumpGuide(const CJumpGuide& Prototype);
	virtual ~CJumpGuide() = default;

public:
	void Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear();
	void		Disappear();

public:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	CGameObject* m_pOwner = { nullptr };

	_float4		m_vColor = {};
	_float		m_fTexCoordY = {};

private:
	HRESULT Ready_Components();

public:
	static CJumpGuide* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END