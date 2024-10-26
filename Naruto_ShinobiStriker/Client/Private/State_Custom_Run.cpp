#include "stdafx.h"
#include "State_Custom_Run.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"

#include "Navigation.h"

CState_Custom_Run::CState_Custom_Run(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Run::Initialize(_uint iStateNum)
{
	m_iAnimation_RunLoop = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Run_Loop");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_Run::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_RunLoop, true);
	m_pPlayer->Appear_FootTrail();

	return S_OK;
}

void CState_Custom_Run::Update(_float fTimeDelta)
{
	Playe_Sound();

	if (false == Move(fTimeDelta))
	{
		m_pPlayer->Change_State(CPlayer_Custom::RUN_END);
	}
	
	if (!m_pPlayer->Get_IsWall())
	{
		if (!Check_Ground(fTimeDelta))
		{
   			m_pPlayer->Change_State(CPlayer_Custom::FALL_FRONT);
		}
	}

	if (KEY_TAP(KEY::SPACE))
	{
		m_pPlayer->Change_State(CPlayer_Custom::JUMP);
		return;
	}

	if (KEY_TAP(KEY::LSHIFT))
	{
		m_pPlayer->Change_State(CPlayer::DASH);
	}
}

void CState_Custom_Run::End_State()
{
}

_bool CState_Custom_Run::Move(_float fTimeDelta)
{
	_bool isMoving = false;
	_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	if (KEY_HOLD(KEY::W))
	{
		vDir += m_pPlayer->Calculate_Direction_Straight();
		isMoving = true;
	}
	else if (KEY_HOLD(KEY::S))
	{
		vDir -= m_pPlayer->Calculate_Direction_Straight();
		isMoving = true;
	}

	if (KEY_HOLD(KEY::D))
	{
		vDir += m_pPlayer->Calculate_Direction_Right();

		isMoving = true;
	}

	if (KEY_HOLD(KEY::A))
	{
		vDir -= m_pPlayer->Calculate_Direction_Right();

		isMoving = true;
	}

	if (XMVector3Length(vDir).m128_f32[0] > 0.f)
	{	
		CNavigation* pNavi = dynamic_cast<CNavigation*>(m_pPlayer->Find_Component(COMPONENT_TYPE::NAVIGATION));
		m_pPlayer->Move_Dir(vDir, 10.0f, fTimeDelta, pNavi);
	}

	return isMoving;
}

void CState_Custom_Run::Playe_Sound()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_RunLoop);

	_uint iSoundFrame1 = 1;
	_uint iSoundFrame2 = 7;

	if (iFrame == iSoundFrame1 || iFrame == iSoundFrame1 + 1 
		|| iFrame == iSoundFrame2 || iFrame == iSoundFrame2 + 1)
	{
		if (!m_isPlaySound)
		{
			m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Run.wav"), g_fVolume);
			m_isPlaySound = true;
		}
	}
	else
	{
		m_isPlaySound = false;
	}
}

_bool CState_Custom_Run::Check_Ground(_float fTimeDelta)
{
	_vector vHitPos, vNormal;
	_float fHitDistance;

	if (m_pGameInstance->RayCast_PlayerDown(&vHitPos, &vNormal, &fHitDistance))
	{
		if (fHitDistance <= 2.5f)
		{
			return true;
		}

	}

	return false;
}

CState_Custom_Run* CState_Custom_Run::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Run* pInstance = new CState_Custom_Run(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Run::Free()
{
	__super::Free();
}
