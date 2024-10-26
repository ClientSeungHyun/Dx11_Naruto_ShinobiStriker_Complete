#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CKuani_Minato final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC {
		const _float4x4* pWorldMatrix;
		const _float4x4* pSocketMatrix;
	}MINATOKUNAI_DESC;

private:
	CKuani_Minato(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKuani_Minato(const CKuani_Minato& Prototype);
	virtual ~CKuani_Minato() = default;

public:
	void Set_HitEffectDuration(_float fTime) { m_HitEffectDuration = fTime; }
	CGameObject* Get_TargetObject() { return m_pTargetObject; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;

public:
	void		Appear();
	void		Disappear();
	void		Shoot(_fvector vShootPos, _float fSpeed, _bool isSettingPos = false, _bool isRaijin = false);


	void		Calculate_Position();

public:
	class CShader*		m_pShaderCom = { nullptr };
	class CModel*		m_pModelCom = { nullptr };
	class CCollider*	m_pColliderCom = { nullptr };
	CGameObject*		m_pTargetObject = { nullptr };

	_float		m_fCurrentTime = {};
	_float		m_HitEffectTime = {};
	_float		m_HitEffectDuration = {};
	_float		m_fSpeed = {};
	_float3		m_vTargetPos = {};
	
	_bool		m_isShoot = { false };
	_bool		m_isRaijin = { false };

	const _float4x4*	m_pWorldMatirx = { nullptr };
	const _float4x4*	m_pSocketMatrix = {nullptr};

	class CChidoriTrail* m_pTrail = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CKuani_Minato* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END