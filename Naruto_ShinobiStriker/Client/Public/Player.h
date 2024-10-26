#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
class CCamera;
class CRigidBody;
class CCollider;
class CSound;
END

BEGIN(Client)

class CPlayer : public CContainerObject
{
public:
	enum SKILLSLOT { SLOT_SP, SLOT_WOOD, SLOT_BASE1, SLOT_BASE2, SLOT_END };

	enum SKILL_TYPE { SKILL_SASUKE, SKILL_NARUTO, SKILL_SARIN, SKILL_MINATO, SKILL_END };

	enum PLAYERSTATE {
		CHANGEWOOD,
		IDLE,
		RUN,
		RUN_END,
		JUMP,
		DOUBLEJUMP,
		FALL_FRONT,
		FALL_BEHIND,
		LAND,
		WALLATTACH,
		CHAKRAJUMP,
		DASH,

		PUNCH_LEFT,
		PUNCH_RIGHT,
		SLASH,
		DOUBLESLASH,
		GROUNDPUNCH,
		RISINGKICK,
		HANDDOUBLEPUNCH,

		HIT1,
		HIT2,
		HIT_SPIN,

		WIRE_READY,
		WIRE_DASH,

		PLAYERSTATE_END
	};

public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3		vPosition = _float3(0.f, 0.f, 0.f);
		_float3		vScale = _float3(1.f, 1.f, 1.f);
		_float3		vRotation = _float3(0.f, 0.f, 0.f);
	}PLAYER_DESC;

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	class CModel*			Get_Model() { return m_pModelCom; }

	_float					Get_MoveSpeed() { return m_fMoveSpeed; }
	void					Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }

	_float					Get_AnimationSpeed() { return m_fAnimationSpeed; }
	void					Set_AnimationSpeed(_float fAnimationSpeed) { m_fAnimationSpeed = fAnimationSpeed; }

	class CPlayerCamera*			Get_Camera() { return m_pPlayerCamera; }
	void					Set_Camera(class CPlayerCamera* pCamera) { m_pPlayerCamera = pCamera; }

	class CRigidBody*		Get_RigidBody() { return m_pRigidBodyCom; }

	class CSound*			Get_SoundCom() { return m_pSoundCom; }
	class CSound*			Get_EffectSoundCom() { return m_pEffectSoundCom; }

	class CCollider*				Get_Collider() { return m_pColliderCom; }
	class CCollider_PlayerAttack*	Get_AttackCollider() { return m_pCollider_Attack; }

	class CFsm*				Get_Fsm() { return m_pFsmCom; }

	class CWireTrail*		Get_WireTrail() { return m_pWireTrail; }

	_bool					Get_IsFall() { return m_isFall; }
	void					Set_IsFall(_bool isFall) { m_isFall = isFall; }

	_bool					Get_IsJump() { return m_isJump; }
	void					Set_IsJump(_bool isJump) { m_isJump = isJump; }

	_bool					Get_IsDoubleJump() { return m_isDoubleJump; }
	void					Set_IsDoubleJump(_bool isDoubleJump) { m_isDoubleJump = isDoubleJump; }

	_bool					Get_IsSkill() { return m_isSkill; }
	void					Set_IsSkill(_bool isSkill) { m_isSkill = isSkill; }

	_bool					Get_IsInvincible() { return m_isInvincible; }
	void					Set_IsInvincible(_bool isInvincible) { m_isInvincible = isInvincible; }

	_bool					Get_IsStrongAttack() { return m_isStrongAttack; }
	void					Set_IsStrongAttack(_bool isStrong) { m_isStrongAttack = isStrong; }

	_bool					Get_IsControl() { return m_isControl; }
	void					Set_IsControl(_bool isControl) { m_isControl = isControl; }

	CGameObject*			Get_NearTarget() { return m_pNearTarget; }
	void					Find_NearTarget();

	_uint					Get_CurrentAnimationIndex();
	_char*					Get_CurrentAnimationName();
	_bool					Get_IsEndAnimation(_uint iAnimationIndex);
	_uint					Get_Frame(_uint iAnimIndex);

	_float3					Get_RootBonePos() { return m_vRootBonePos; }

	void					Change_SkillType(SKILL_TYPE eType);

	SKILL_TYPE				Get_SkillType() { return m_eSkillType; }

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
	virtual void	Change_IsRender(_bool isRender) { m_isRender = isRender; }

	void			Move_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void			Change_State(const _uint iState, void* pArg = nullptr);
	void			SetUp_NextAnimation(_uint iNextAnimationIndex, _bool isLoop = false, _float fChangeDuration = 0.2f, _uint iStartFrame = 0);

	void			Chase_Target(_float fTimeDelta, _float fMoveSpeed);

	void			Go_Straight(_float fTimeDelta, _float fSpeed);
	_vector			Calculate_Direction_Straight();
	_vector			Calculate_Direction_Right();

	void			Damaged(_float fDamage);
	void			KnockBack(_float fBackPower, _float fUpPower);

	void			Appear_FootTrail();
	void			Disappear_FootTrail();

	void			Appear_HitEffect(_fvector vColor, _uint iNumParticle = 50);
	
	void			Add_HitEffect();

	void			Appear_JumpGuide();
	void			Disappear_JumpGuide();

	void			Start_AppearUI();
	void			Start_DisappearUI();

	void			Scaled_AttackCollider(_float fX, _float fY, _float fZ);
	void			Offset_AttackCollider(_float fX, _float fY, _float fZ);

	void			Appear_MinatoKunai();
	void			Shoot_MinatoKunai();
	void			Move_KunaiCenter();
	void			Disappear_MinatoKunai();

	void			Appear_MinatoKunai_One();
	void			Shoot_MinatoKunai_One();
	void			Disappear_MinatoKunai_One();
	_bool			Get_IsKunaiCollision_One();
	CGameObject*	Get_KuaniHitObject_One();

protected:
	class CModel*		m_pModelCom = { nullptr };
	class CShader*		m_pShaderCom = { nullptr };
	class CFsm*			m_pFsmCom = { nullptr };
	class CRigidBody*	m_pRigidBodyCom = { nullptr };
	class CCollider*	m_pColliderCom = { nullptr };
	class CSound*		m_pSoundCom = { nullptr };	// 목소리용
	class CSound*		m_pEffectSoundCom = { nullptr };	// 효과용

protected:
	class CPlayerCamera*		m_pPlayerCamera = { nullptr };

	SKILL_TYPE			m_eSkillType = SKILL_SASUKE;

	_uint				m_iFootBoneIndex[2] = {};	//좌 우 발
	_uint				m_iLeftHandBoneIndex = {};
	_uint				m_iRightHandBoneIndex = {};
	_uint				m_iHeadBoneIndex = {};
	_uint				m_iWallCount = {};

	_float				m_fCurrentHP = { 100.f };
	_float				m_fMaxHP = { 100.f };
	_float				m_fMoveSpeed = { };
	_float				m_fAnimationSpeed = { 1.f };

	_float						m_fCollTime_Skill1 = { 0.f };
	_float						m_fCollTime_Skill2 = { 0.f };
	_float						m_fCollTime_SkillSP = { 0.f };

	_bool				m_isFall = { false };
	_bool				m_isJump = { false };
	_bool				m_isSkill = { false };
	_bool				m_isDoubleJump = { false };
	_bool				m_isInvincible = { false };
	_bool				m_isShadow = { false };
	_bool				m_isStrongAttack = { false };
	_bool				m_isControl = { true };

	_float3				m_vRootBonePos = {};
	_float3				m_vPxHitDir = { 0.f,0.f,0.f };

	CGameObject*		m_pNearTarget = { nullptr };
	CGameObject*		m_pHitObject = { nullptr };

	class CCollider_PlayerAttack* m_pCollider_Attack = { nullptr };

	class CSkill_ChangeWood*	m_pSkill_ChangeWood = { nullptr };
	_float						m_fCoolTime_ChangeWood = 0.f;

	class CUI_PlayerSP*			m_pUI_SP = { nullptr };
	class CUI_PlayerHP*			m_pUI_HP = { nullptr };
	class CUI_PlayerSkill*		m_pUI_SkillSlot[SLOT_END] = { nullptr, nullptr, nullptr, nullptr };
	class CUI_HitNum*			m_pUI_Hit = { nullptr };
	class CUI_Target*			m_pUI_Target = { nullptr };

	class CFootTrail*			m_pFootTrail[2] = { nullptr };
	class CWireTrail*			m_pWireTrail = { nullptr };
	class CJumpGuide*			m_pJumpGuide = { nullptr };

	class CKuani_Minato*		m_pMinatoKunai[6] = { nullptr };

protected:
	HRESULT Ready_Components();
	HRESULT Ready_Collider();
	HRESULT Ready_Skill();
	HRESULT Ready_UI();
	HRESULT Ready_ETC();

	void Control_Gravity();
	_bool Change_Wood();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END