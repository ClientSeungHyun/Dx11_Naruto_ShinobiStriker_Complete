#pragma once

#include "Client_Defines.h"
#include "Player.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC{
		CGameObject* pOwner;
		_bool isSpecialSkill = false;
	}SKILL_DESC;

protected:
	CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill(const CSkill& Prototype);
	virtual ~CSkill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};

public:
	void					Find_NearTarget();
	void					Move_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta);

	void					Chanse_Target(_float fTimeDelta);

protected:
	class CCollider*		m_pColliderCom = { nullptr };
	class CSound*			m_pSoundCom = { nullptr };

	CGameObject*			m_pOwner = { nullptr };
	CGameObject*			m_pNearTarget = { nullptr };

	_float						m_fCurrentTime = { 0.f };
	
	_char						m_szPositionBoneName[MAX_PATH] = "";
	const _float4x4*			m_pSocketMatrix = { nullptr };

protected:
	HRESULT Ready_Components();

public:
	static CSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END