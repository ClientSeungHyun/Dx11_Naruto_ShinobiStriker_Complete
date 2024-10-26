#pragma once

#include "Base.h"
#include "Transform.h"

/* ��� ���ӳ��� ���Ǵ� ���� ������Ʈ���� �θ� Ŭ������. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{

	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	_bool		Get_Dead() { return m_isDead; }
	void		Set_Dead(_bool isDead) { m_isDead = isDead; }

	_bool		IsActive() { return m_isActive; }
	void		IsActive(_bool isActive) { m_isActive = isActive; }

	_bool		Get_IsPicked() { return m_isPicked; }
 
	void		Set_IsPicked(_bool isPicked) { m_isPicked = isPicked; }

	class CTransform* Get_Transform() {
		if (m_pTransformCom)
			return m_pTransformCom;
		return nullptr;
	}

	_vector Get_Position() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

	_float Get_Distance_To_Camera() const { return m_fDistance_To_Camera; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual _bool Is_Pick(_float3* vPickPos) { return false; }

public:
	class CComponent* Find_Component(const _wstring& strComponentTag);
	class CComponent* Find_Component(const COMPONENT_TYPE& eComponentType);

	void		Compute_Distance_To_Camera();
	_float		Compute_Distance(_fvector vPos);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	_bool						m_isActive = { true };
	_bool						m_isPicked = { false };
	_bool						m_isCloned = { false };
	_bool						m_isDead = { false };

	_float						m_fDistance_To_Camera = {};

protected:
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END