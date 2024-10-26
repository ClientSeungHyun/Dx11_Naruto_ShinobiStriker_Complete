#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CFsm;
class CRigidBody;
class CCollider;
class CSound;
END


BEGIN(Client)
class CMonster : public CGameObject
{
public:
	enum MONSTERSTATE
	{
		APPEAR,
		IDLE,
		DIE,

		MONSTER_END
	};
public:
	typedef struct :CGameObject::GAMEOBJECT_DESC
	{
		_char		szLayerTag[MAX_PATH];
		_char		szPrototypeTag[MAX_PATH];
		_char		szModelTag[MAX_PATH];
		_float3		vPosition;
		_float3		vScale;
		_float3		vRotation;
	}MONSTER_DESC;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	class CModel*			Get_Model() { return m_pModelCom; }
	class CFsm*				Get_Fsm() { return m_pFsmCom; }
	class CRigidBody*		Get_RigidBody() { return m_pRigidBodyCom; }
	class CSound*			Get_SoundCom() { return m_pSoundCom; }
	class CSound*			Get_EffectSoundCom() { return m_pEffectSoundCom; }

	class CCollider*		Get_Collider() { return m_pColliderCom; }
	class CColliderObject*	Get_AttackCollider() { return m_pCollider_Attack; }

	class CGameObject*		Get_PlayerTarget() { return m_pPlayerTarget; }

	_float					Get_AnimationSpeed() { return m_fAnimationSpeed; }
	void					Set_AnimationSpeed(_float fAnimationSpeed) { m_fAnimationSpeed = fAnimationSpeed; }

	_uint					Get_CurrentAnimationIndex();
	_char*					Get_CurrentAnimationName();
	_bool					Get_IsEndAnimation(_uint iAnimationIndex);
	_uint					Get_Frame(_uint iAnimIndex);

	_bool					Get_IsInvincible() { return m_isInvincible; }
	void					Set_IsInvincible(_bool isInvincible) { m_isInvincible = isInvincible; }

	_bool					Get_IsStrongAttack() { return m_isStrongAttack; }
	void					Set_IsStrongAttack(_bool isStrong) { m_isStrongAttack = isStrong; }

	_bool					Get_IsCheckPhysX() { return m_isCheckPhysX; }
	void					Set_IsCheckPhysX(_bool isCheck) { m_isCheckPhysX = isCheck; }

	_float					Get_CurrentHP() { return m_fCurrentHP; }

	MONSTER_ID				Get_MonsterID() { return m_eMonsterID; }

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
	virtual void		Damaged_To_Player(_uint iState, _float fDamage = 10.f, _bool isStrongAttack = false) {};
	void				Damaged(_float fDamage);

	void				Move_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta);
	void				Change_State(const _uint iState);
	void				SetUp_NextAnimation(_uint iNextAnimationIndex, _bool isLoop = false, _float fChangeDuration = 0.2f, _uint iStartFrame = 0);
	void				Force_BackWard(_float fForcePower = 6.f);
	void				Force_RandomBackWard(_float fForcePower = 6.f);
	void				Force_BackUp(_float fBackePower = 6.f, _float fUpPower = 6.f);
	void				Find_PlayerTarget();
	void				Chase_Player(_float fTimeDelta, _float fMoveSpeed);

	virtual void		Change_State_GeneralHit() {};
	virtual void		Change_State_FlyAway() {};
	virtual void		Change_State_FlyGround() {};
	virtual void		Change_State_Groggy() {};
	virtual void		Change_State_HitDown() {};

	_float		Carculate_TargetDir();

protected:
	class CModel*			m_pModelCom = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };
	class CFsm*				m_pFsmCom = { nullptr };
	class CRigidBody*		m_pRigidBodyCom = { nullptr };
	class CSound*			m_pSoundCom = { nullptr };
	class CSound*			m_pEffectSoundCom = { nullptr };
	class CCollider*		m_pColliderCom = { nullptr };
	class CColliderObject*	m_pCollider_Attack = { nullptr };

	CGameObject*			m_pPlayerTarget = { nullptr };
	class CUI_MonsterHP*	m_pUI_HP = { nullptr };

	_float3					m_vRootBonePos = {};

protected:
	_float					m_fMoveSpeed = {};
	_float					m_fAttackMoveSpeed = {};
	_float					m_fAnimationSpeed = { 1.f };

	_float					m_fCurrentHP = { 100.f };
	_float					m_fMaxHP = { 100.f };

	_bool					m_isInvincible = { false };
	_bool					m_isStrongAttack = { false };
	_bool					m_isCheckPhysX = { false };

	MONSTER_ID				m_eMonsterID = { MON_END };

protected:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_UI();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END

