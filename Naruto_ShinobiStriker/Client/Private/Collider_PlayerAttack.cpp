#include "stdafx.h"
#include "ColliderObject.h"

#include "GameInstance.h"
#include "Collider_PlayerAttack.h"
#include "Player_Custom.h"
#include "Monster.h"
#include "PlayerCamera.h"

#include "ObjectPool.h"
#include "Effect_Distortion.h"

CCollider_PlayerAttack::CCollider_PlayerAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CColliderObject{ pDevice, pContext }
{
}

HRESULT CCollider_PlayerAttack::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strLayerTag = TEXT("Layer_PlayerAttack");
	m_isActive = false;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pOwner);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	return S_OK;
}

void CCollider_PlayerAttack::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCollider_PlayerAttack::Update(_float fTimeDelta)
{
	m_pTransformCom->Set_WorldMatrix(m_pOwner->Get_Transform()->Get_WorldMatrix());
	m_pTransformCom->Set_Scaled(m_vScale.x, m_vScale.y, m_vScale.z);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vOffset = XMLoadFloat3(&m_vOffset);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vOffset);

	__super::Update(fTimeDelta);
}

void CCollider_PlayerAttack::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCollider_PlayerAttack::Render()
{
	__super::Render();

	return S_OK;
}

void CCollider_PlayerAttack::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);

		// 공통적 애니메이션 바꿔줄 것
		if (m_pPlayer->Get_Fsm()->Get_CurrentState() == CPlayer_Custom::CHIDORI_RUN)
		{
			m_pPlayer->Change_State(CPlayer_Custom::CHIDORI_ATTACK);
		}
		else if (m_pPlayer->Get_Fsm()->Get_CurrentState() == CPlayer_Custom::AERIAL_CHIDORI_RUN)
		{
			m_pPlayer->Change_State(CPlayer_Custom::AERIAL_CHIDORI_ATTACK);
		}
		else if (m_pPlayer->Get_Fsm()->Get_CurrentState() == CPlayer_Custom::RASENGUN_RUN)
		{
			m_pPlayer->Change_State(CPlayer_Custom::RASENGUN_ATTACK);
		}
		else if (m_pPlayer->Get_Fsm()->Get_CurrentState() == CPlayer_Custom::AERIAL_RASENGUN_RUN)
		{
			m_pPlayer->Change_State(CPlayer_Custom::AERIAL_RASENGUN_ATTACK);
		}
		

		Enter_BaseAttack(pMonster);
		
		if (m_pPlayer->Get_Fsm()->Get_CurrentState() == CPlayer_Custom::RASENKO)
		{
			if (pMonster->Get_MonsterID() != KURAMA && pMonster->Get_MonsterID() != SUSANOO)
			{
				pMonster->Change_State_FlyAway();
				pMonster->Force_BackUp(7.f, 10.f);
			}
			pMonster->Damaged(30.f);
			m_pPlayer->Add_HitEffect();
		}
		else if (m_pPlayer->Get_Fsm()->Get_CurrentState() == CPlayer_Custom::TYPEZERO)
		{
			m_pPlayer->Add_HitEffect();

			if (m_pPlayer->Get_IsStrongAttack())
			{
				pMonster->Damaged(15.f);
				pMonster->Change_State_FlyGround();
			}
			else
			{
				pMonster->Damaged(5.f);
				pMonster->Change_State_GeneralHit();
			}
		}

		m_pPlayer->Add_HitEffect();
		m_pPlayer->Appear_HitEffect(XMVectorSet(0.f, 0.05f, 0.9f, 1.f));
	}
}

void CCollider_PlayerAttack::OnCollisionStay(CGameObject* pOther)
{
}

void CCollider_PlayerAttack::OnCollisionExit(CGameObject* pOther)
{
}

void CCollider_PlayerAttack::Enter_BaseAttack(CMonster* pOther)
{
	_uint iCurrentState = m_pPlayer->Get_Fsm()->Get_CurrentState();

	if (iCurrentState == CPlayer_Custom::PUNCH_LEFT	
		|| iCurrentState == CPlayer_Custom::PUNCH_RIGHT
		|| iCurrentState == CPlayer_Custom::DOUBLESLASH
		|| iCurrentState == CPlayer_Custom::SLASH)
	{
		if(iCurrentState == CPlayer_Custom::PUNCH_LEFT)
			m_pSoundCom->Play3D(TEXT("Effect_Attack1.wav"), g_fVolume);
		else if (iCurrentState == CPlayer_Custom::PUNCH_RIGHT)
			m_pSoundCom->Play3D(TEXT("Effect_Attack2.wav"), g_fVolume);
		else if (iCurrentState == CPlayer_Custom::DOUBLESLASH)
			m_pSoundCom->Play3D(TEXT("Effect_AttackSlash.wav"), g_fVolume);
		else if (iCurrentState == CPlayer_Custom::SLASH)
			m_pSoundCom->Play3D(TEXT("Effect_AttackSlash.wav"), g_fVolume);


		if (pOther->Get_MonsterID() == WHITEZETSU)
		{
			pOther->Change_State_GeneralHit();
			pOther->Force_BackWard();
			pOther->Damaged(5.f);

			Appear_Distortion(pOther);
		}

	}
	else if (iCurrentState == CPlayer_Custom::GROUNDPUNCH)
	{
		m_pSoundCom->Play3D(TEXT("Effect_AttackGround.wav"), g_fVolume);
		if (pOther->Get_MonsterID() == WHITEZETSU)
		{
			pOther->Change_State_FlyAway();
			pOther->Force_BackUp(6.f, 10.f);
			pOther->Damaged(15.f);
		}

		m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->ZoomIn(-0.5f, 0.f);
		m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->Start_Shake(0.6f, 0.05f);
		m_pGameInstance->Start_Lack(TEXT("Timer_60"), 30);

		Appear_Distortion(pOther);
	}
	else if (iCurrentState == CPlayer_Custom::RISINGKICK)
	{
		m_pSoundCom->Play3D(TEXT("Effect_Attack0.wav"), g_fVolume);
		if (pOther->Get_MonsterID() == WHITEZETSU)
		{
			pOther->Change_State_GeneralHit();
			pOther->Set_IsGravity(true);
			pOther->Force_BackUp(1.5f, 10.f);
			pOther->Damaged(10.f);

			_matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();

			_vector vPlayerRight = XMVector3Normalize(PlayerWorldMatrix.r[0]);
			_vector vPlayerUp = XMVector3Normalize(PlayerWorldMatrix.r[1]);
			_vector vPlayerLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);
			_vector vPlayerPos = PlayerWorldMatrix.r[3];

			_vector vCameraPos = vPlayerPos + (vPlayerUp * 0.5f) - (vPlayerLook * 2.f) + (vPlayerRight) * 1.4f;
			_vector vCameraEndPos = vPlayerPos + (vPlayerUp * 7.f) - (vPlayerLook * 2.5f) - (vPlayerRight) * 1.8f;
			m_pPlayer->Get_Camera()->Setting_CameraControl(vCameraPos, vCameraEndPos, vPlayerPos + vPlayerUp * 1.5f + vPlayerLook * 2.f);
			m_pPlayer->Get_Camera()->Start_CameraControl();
			m_pPlayer->Get_Camera()->Start_CameraLerp(0.5f);
			m_pGameInstance->Start_Lack(TEXT("Timer_60"), 10);
			m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->Start_Shake(0.6f, 0.05f);

			Appear_Distortion(pOther);
		}
	}
	else if (iCurrentState == CPlayer_Custom::HANDDOUBLEPUNCH)
	{
		m_pSoundCom->Play3D(TEXT("Effect_AttackTwoHand.wav"), g_fVolume);
		if (pOther->Get_MonsterID() == WHITEZETSU)
		{
			pOther->Change_State_HitDown();
			pOther->Set_IsGravity(true);
			pOther->Force_BackUp(8.f, -15.f);
			pOther->Damaged(15.f);
			m_pGameInstance->Start_Lack(TEXT("Timer_60"), 35);
			m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->Start_Shake(0.6f, 0.05f);

			Appear_Distortion(pOther);
		}
	}
	//_vector vJumpDir;
	//_vector vJumpPower = XMVectorSet(3.f, 12.f, 3.f, 0.f);

	//_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//_vector vLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	//vJumpDir = XMVector3Normalize((vLook * 0.2f) + vUp);
}

void CCollider_PlayerAttack::Collision_Boss(CMonster* pBoss)
{
}

void CCollider_PlayerAttack::Collision_Monster(CMonster* pMonster)
{
}

void CCollider_PlayerAttack::Appear_Distortion(CMonster* pMonster)
{
	_matrix WorldMatrix = pMonster->Get_Transform()->Get_WorldMatrix();

	_vector vUp = XMVector3Normalize(WorldMatrix.r[1]);
	_vector vPos = WorldMatrix.r[3];

	vPos = vPos + vUp * 1.f;
	CObjectPool<CEffect_Distortion>::Get_GameObject()->Appear(vPos, 0.1f, 6.f, 15.f, 0.3f, CEffect_Distortion::DIST_RING);
}

CCollider_PlayerAttack* CCollider_PlayerAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CCollider_PlayerAttack* pInstance = new CCollider_PlayerAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_PlayerAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_PlayerAttack::Free()
{
	__super::Free();
	Safe_Release(m_pSoundCom);
}
