#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CChidori_Center : public CEffect
{

private:
	CChidori_Center(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChidori_Center(const CChidori_Center& Prototype);
	virtual ~CChidori_Center() = default;

public:
	void		Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }

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
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pOwner = { nullptr };

	_float			m_fCurrentTime = { 0.f };
	_float3			m_vSize = {};
	_float2			m_vTexCoord = {};

private:
	virtual HRESULT Ready_Components();

public:
	static CChidori_Center* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END