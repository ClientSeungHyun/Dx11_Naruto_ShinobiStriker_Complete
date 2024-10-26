#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Trail_Instance;
END

BEGIN(Client)

class CFootTrail : public CEffect
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC {
		const _float4x4* m_pOwnerWorldMatrix;
		const _float4x4* m_pSocketMatrix;

	}FOOTTRAIL_DESC;

private:
	CFootTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFootTrail(const CFootTrail& Prototype);
	virtual ~CFootTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear();
	void		DisAppear();

private:
	class CVIBuffer_Trail_Instance*		m_pVIBufferCom = { nullptr };

	_bool						m_isDisappear = { false };

	_float						m_fAddPointTime = { 0.f };

	const _float4x4*			m_PlayerWorldMatrix = { nullptr };
	const _float4x4*			m_pSocketMatrix = { nullptr };

private:
	virtual HRESULT Ready_Components();

public:
	static CFootTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END