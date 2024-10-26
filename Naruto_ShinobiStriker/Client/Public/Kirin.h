#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CKirin : public CGameObject
{
private:
	CKirin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKirin(const CKirin& Prototype);
	virtual ~CKirin() = default;

public:
	class CModel* Get_Model() { return m_pModelCom; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Appear(_fvector vPos);
	void Disappear();

private:
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };

	_uint m_iAnimationIndex = {};

	_float4 m_vColor = {};

private:
	HRESULT Ready_Components();

public:
	static CKirin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END