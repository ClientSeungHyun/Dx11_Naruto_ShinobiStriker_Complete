#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CSound;
END

BEGIN(Client)

class CAreaCircle final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC{
		_float3 vPos;
	}AREACIRCLE_DESC;

private:
	CAreaCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAreaCircle(const CAreaCircle& Prototype);
	virtual ~CAreaCircle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_fvector vPos);
	void		Disappear();

public:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };
	class CSound* m_pSoundCom = { nullptr };

	_float		m_fCurrentTime = {};

	_float3		m_vInitSize = {};
	_float4		m_vColor = {};

	_bool		m_isPlaySound = { false };

private:
	HRESULT Ready_Components();

public:
	static CAreaCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END