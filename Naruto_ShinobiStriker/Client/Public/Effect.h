#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CEffect : public CGameObject
{

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	class CModel*			m_pModelCom = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };
	class CTexture*			m_pTextureCom = { nullptr };

	_float4					m_vEffectColor = { 0.f,0.f,0.f,0.f };
	_float4					m_vMaskColor = { 0.f,0.f,0.f,0.f };

protected:
	virtual HRESULT Ready_Components();
	void			Look_Camera();

public:
	virtual void Free();
};

END