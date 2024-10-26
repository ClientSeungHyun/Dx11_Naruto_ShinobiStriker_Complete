#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWood final : public CGameObject
{
private:
	CWood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWood(const CWood& Prototype);
	virtual ~CWood() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_fmatrix WorldMatrix);

public:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_float			m_fAlpha = { 0.f };
	_float			m_fAliveTime = { 0.f };
	_float			m_fAliveDuration = { 2.f };

	_bool			m_isEnd = { false };

private:
	HRESULT Ready_Components();

public:
	static CWood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END