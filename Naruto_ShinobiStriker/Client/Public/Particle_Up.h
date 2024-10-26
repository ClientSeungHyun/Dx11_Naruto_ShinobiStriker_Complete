#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Up final : public CGameObject
{
public:
	enum PARTICLE_TYPE { PARTICLE_SPRED, PARTICLE_UP, PARTICLE_DROP, PARTICLE_END };

private:
	CParticle_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Up(const CParticle_Up& Prototype);
	virtual ~CParticle_Up() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Appear(_fvector vPos, _fvector vColor, PARTICLE_TYPE eType = PARTICLE_SPRED);

public:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

	_float	m_fCurrentTime = {};

	_float4 m_vColor = {};

	PARTICLE_TYPE	m_eType = { PARTICLE_END };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Up* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END