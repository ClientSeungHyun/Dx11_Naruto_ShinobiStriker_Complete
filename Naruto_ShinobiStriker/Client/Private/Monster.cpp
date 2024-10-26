#include "stdafx.h"
#include "Monster.h"

#include "UI_MonsterHP.h"

#include "ColliderObject.h"
#include "GameInstance.h"

#include "Boss_Kurama.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
    : CGameObject{ Prototype }
{
}

_uint CMonster::Get_CurrentAnimationIndex()
{
	return m_pModelCom->Get_CurrentAnimationIndex();
}

_char* CMonster::Get_CurrentAnimationName()
{
	return m_pModelCom->Get_CurrentAnimationName();
}

_bool CMonster::Get_IsEndAnimation(_uint iAnimationIndex)
{
	return m_pModelCom->Get_IsEnd_Animation(iAnimationIndex);
}

_uint CMonster::Get_Frame(_uint iAnimIndex)
{
	return m_pModelCom->Get_Frame(iAnimIndex);
}

HRESULT CMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pDesc)
	{
		m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	}

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
}

void CMonster::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	if(m_eMonsterID == KURAMA && m_pFsmCom->Get_CurrentState() == CBoss_Kurama::SPECIALATTACK)
		_vector vRootBonePos = m_pModelCom->Play_Animation(fTimeDelta * m_fAnimationSpeed, 4);
	else
		_vector vRootBonePos = m_pModelCom->Play_Animation(fTimeDelta * m_fAnimationSpeed);
	//XMStoreFloat3(&m_vRootBonePos, vRootBonePos);
	//_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vNewPos = XMVectorSetY(vCurrentPos, XMVectorGetY(vCurrentPos) + XMVectorGetY(vRootBonePos)); //XMVectorAdd(vCurrentPos, vRootBonePos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

	m_pCollider_Attack->Update(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_isCheckPhysX)
	{
		m_pGameInstance->Add_PhysX_Monster(this);
	}

	if (!m_isCollision && m_isGravity)
	{
		m_pRigidBodyCom->Set_IsGravity(true);
		m_pRigidBodyCom->Set_IsGround(false);
	}
	else if (m_isGround)
	{
		m_pRigidBodyCom->Set_IsGravity(false);
		m_pRigidBodyCom->Set_IsGround(true);
	}

	m_pRigidBodyCom->Update(fTimeDelta);
	m_pCollider_Attack->Late_Update(fTimeDelta);
	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	if (m_isRender)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
		if (m_eMonsterID == KURAMA)
			m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
	}
}

HRESULT CMonster::Render()
{
#ifdef _DEBUG
	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		m_pColliderCom->Render();
		m_pCollider_Attack->Render();
	}
#endif

	return S_OK;
}

void CMonster::Damaged(_float fDamage)
{
	if (m_pFsmCom->Get_CurrentState() == DIE)
		return;

	 m_fCurrentHP -= fDamage; 
}

void CMonster::Move_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta)
{
	m_pTransformCom->Turn_Lerp(vDir, 10.0f, fTimeDelta);
	m_pTransformCom->Go_Straight(fSpeed, fTimeDelta);
}

void CMonster::Change_State(const _uint iState)
{
	m_pFsmCom->Change_State(iState);
}

void CMonster::SetUp_NextAnimation(_uint iNextAnimationIndex, _bool isLoop, _float fChangeDuration, _uint iStartFrame)
{
	m_pModelCom->SetUp_NextAnimation(iNextAnimationIndex, isLoop, fChangeDuration, iStartFrame);
}

void CMonster::Force_BackWard(_float fForcePower)
{
	Find_PlayerTarget();

	_vector vBack;

	if (nullptr != m_pPlayerTarget)
	{
		_vector vPlayerPos = XMVectorSetY(m_pPlayerTarget->Get_Position(), 0.f);
		_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);

		vBack = XMVector3Normalize(vPos - vPlayerPos);

	}
	else
		vBack = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * -1.f;

	m_pRigidBodyCom->Clear_All();
	m_pRigidBodyCom->Add_Force(CRigidBody::FORCE, vBack * fForcePower);
}

void CMonster::Force_RandomBackWard(_float fForcePower)
{
	_float3 vRandBack = _float3(m_pGameInstance->Get_Random(-1.f, 1.f), 0.f, m_pGameInstance->Get_Random(-1.f, 1.f));
	_vector vBack = XMVector3Normalize(XMLoadFloat3(&vRandBack));

	m_pRigidBodyCom->Clear_All();
	m_pRigidBodyCom->Add_Force(CRigidBody::FORCE, vBack * fForcePower);
}

void CMonster::Force_BackUp(_float fBackPower, _float fUpPower)
{
	Find_PlayerTarget();

	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vBack;

	if (nullptr != m_pPlayerTarget)
	{
		_vector vPlayerPos = XMVectorSetY(m_pPlayerTarget->Get_Position(), 0.f);
		_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);

		vBack = XMVector3Normalize(vPos - vPlayerPos);
	}
	else
		vBack = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * -1.f;

	m_pRigidBodyCom->Clear_All();
	m_pRigidBodyCom->Add_Force(CRigidBody::FORCE, vBack * fBackPower);
	m_pRigidBodyCom->Add_Force(CRigidBody::FORCE, vUp * fUpPower);
}

void CMonster::Find_PlayerTarget()
{
	Safe_Release(m_pPlayerTarget);
	m_pPlayerTarget = nullptr;

	m_pPlayerTarget = m_pGameInstance->Find_Player(m_iLevelIndex);
	Safe_AddRef(m_pPlayerTarget);
}

void CMonster::Chase_Player(_float fTimeDelta, _float fMoveSpeed)
{
	if (nullptr == m_pPlayerTarget)
		return;

	_vector vPos = Get_Position();
	_vector vTargetPos = m_pPlayerTarget->Get_Position();

	// 상하에 관여받지 않는 dir을 구해야함
	_vector vDir = vTargetPos - vPos;

	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE::STATE_UP);
	vUp = XMVector3Normalize(vUp);

	// Up 벡터 방향으로 얼마만큼 가는지 계산함
	_float fLength = -1.f;
	XMStoreFloat(&fLength, XMVector3Dot(vDir, vUp));

	// 그 길이만큼 up 벡터와 곱해서 뺌
	vTargetPos = vTargetPos - (vUp * fLength);
	vDir = vTargetPos - vPos;

	// 이동하는 양을 구함
	_vector vMove = vDir;
	vMove = XMVector3Normalize(vMove);;
	vMove *= fMoveSpeed * fTimeDelta;

	_float fDistance, fMoveAmount;

	// dir은 거리 move는 실제 움직이는 양
	XMStoreFloat(&fDistance, XMVector3Length(vDir));
	XMStoreFloat(&fMoveAmount, XMVector3Length(vMove));

	m_pTransformCom->Turn_Lerp(vDir, 10.0f, fTimeDelta);

	// 거리거 더 멀면 이동
	if (fDistance > fMoveAmount)
		vPos += vMove;

	m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vPos);
}

_float CMonster::Carculate_TargetDir()
{
	// 두 벡터의 외적이 양수이면 반시계 방향, 음수이면 시계 방향이다.
	// 양수면 오른쪽 음수면 왼쪽
	if (nullptr == m_pPlayerTarget)
		return 0.f;

	_vector vDir = XMVector3Normalize(m_pPlayerTarget->Get_Position() - Get_Position());
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_vector vCross = XMVector3Cross(vLook, vDir);
	_float fDirection = XMVectorGetY(vCross);  // Y값이 양수면 오른쪽, 음수면 왼쪽

	return fDirection;
}

HRESULT CMonster::Ready_Components(void* pArg)
{
	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	/* FOR.Com_EffectSound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_EffectSound"), reinterpret_cast<CComponent**>(&m_pEffectSoundCom))))
		return E_FAIL;
	m_pEffectSoundCom->Set_Owner(this);

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CMonster::MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	string str = pDesc->szModelTag;

	_wstring strModelTag;
	strModelTag.assign(str.begin(), str.end());
	wprintf(strModelTag.c_str());

	str = pDesc->szModelTag;
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(m_iLevelIndex, strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom))))
		return E_FAIL;

	m_pRigidBodyCom->SetOwner(this);
	m_pRigidBodyCom->Set_IsFriction(true);
	m_pRigidBodyCom->Set_Friction(_float3(20.f, 0.f, 20.f));
	m_pRigidBodyCom->Set_IsGravity(false);
	m_pRigidBodyCom->Set_GravityScale(15.f);
	m_pRigidBodyCom->Set_VelocityLimit(_float3(25.f, 30.f, 25.f));

	return S_OK;
}

HRESULT CMonster::Ready_UI()
{
	CUIObject::UI_DESC			UIDesc{};

	UIDesc.iDepth = 1;
	UIDesc.isChild = false;
	UIDesc.iLevelIndex = m_iLevelIndex;
	UIDesc.fSpeedPerSec = 10.f;
	UIDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CUI_MonsterHP::HPUI_DESC		HPUIDesc{};
	memcpy(&HPUIDesc, &UIDesc, sizeof(CUIObject::UI_DESC));

	HPUIDesc.pMonster = this;
	HPUIDesc.pMonsterHP = &m_fCurrentHP;
	m_pUI_HP = dynamic_cast<CUI_MonsterHP*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_MonsterUI"), TEXT("Prototype_UI_MonsterHP"), &HPUIDesc));
	if (nullptr == m_pUI_HP)
		return E_FAIL;
	Safe_AddRef(m_pUI_HP);

	m_pUI_HP->Start_Appear();

	return S_OK;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerTarget);
	Safe_Release(m_pUI_HP);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCollider_Attack);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pSoundCom);
	Safe_Release(m_pEffectSoundCom);
	Safe_Release(m_pRigidBodyCom);
}
