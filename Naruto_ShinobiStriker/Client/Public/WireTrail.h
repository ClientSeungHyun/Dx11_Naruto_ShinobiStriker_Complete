#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Trail_Instance;
END

BEGIN(Client)

class CWireTrail : public CEffect
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC {
		const _float4x4* m_pOwnerWorldMatrix;
		const _float4x4* m_pSocketMatrix;

	}WIRETRAIL_DESC;

private:
	CWireTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWireTrail(const CWireTrail& Prototype);
	virtual ~CWireTrail() = default;

public:
	_float3 Get_EndPos() { return m_vTrailEndPos; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool		Appear();
	void		DisAppear();

private:
	class CVIBuffer_Trail_Instance* m_pVIBufferCom = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

	_float3						m_vTrailEndPos = {};

	_float						m_fTrailSize = {};

	_bool						m_isDisappear = { false };

	const _float4x4*			m_PlayerWorldMatrix = { nullptr };
	const _float4x4*			m_pSocketMatrix = { nullptr };

private:
	virtual HRESULT Ready_Components();

	_bool RayCast();

public:
	static CWireTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END