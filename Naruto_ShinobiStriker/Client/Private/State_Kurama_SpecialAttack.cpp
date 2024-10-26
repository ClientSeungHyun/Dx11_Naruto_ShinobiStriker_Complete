#include "stdafx.h"
#include "State_Kurama_SpecialAttack.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_SpecialAttack::CState_Kurama_SpecialAttack(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_SpecialAttack::Initialize(_uint iStateNum)
{
	m_iAnimation_SpecialAttack = m_pMonster->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_D60NJ1_D61_Kurama", 1.7);
	m_iStateNum = iStateNum;
	m_isFirst = true;

	// 왼쪽부터
	m_iTailedIndex[0] = m_pMonster->Get_Model()->Get_BoneIndex("TailRight04Bone06");
	m_iTailedIndex[1] = m_pMonster->Get_Model()->Get_BoneIndex("TailRight03Bone06");
	m_iTailedIndex[2] = m_pMonster->Get_Model()->Get_BoneIndex("TailRight02Bone06");
	m_iTailedIndex[3] = m_pMonster->Get_Model()->Get_BoneIndex("TailRight01Bone06");
	m_iTailedIndex[4] = m_pMonster->Get_Model()->Get_BoneIndex("TailCenterBone06");
	m_iTailedIndex[5] = m_pMonster->Get_Model()->Get_BoneIndex("TailLeft01Bone06");
	m_iTailedIndex[6] = m_pMonster->Get_Model()->Get_BoneIndex("TailLeft02Bone06");
	m_iTailedIndex[7] = m_pMonster->Get_Model()->Get_BoneIndex("TailLeft03Bone06");
	m_iTailedIndex[8] = m_pMonster->Get_Model()->Get_BoneIndex("TailLeft04Bone06");

	return S_OK;
}

HRESULT CState_Kurama_SpecialAttack::Start_State(void* pArg)
{
	m_iAnimation_SpecialAttack = m_pMonster->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_D60NJ1_D61_Kurama", 1.7);
	m_pMonster->SetUp_NextAnimation(m_iAnimation_SpecialAttack, false, 2.5f);
	m_pMonster->Set_IsRender(true);

	m_fAttackTime = 0.f;
	m_iAttackIndex = 0;

	dynamic_cast<CBoss_Kurama*>(m_pMonster)->Create_Rasengan();

	m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_GoKurama.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Kurama_SpecialAttack::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_SpecialAttack);

	//70 전까지 계속 공격
	m_fAttackTime += fTimeDelta;


	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_SpecialAttack))
	{
		m_pMonster->Change_State(CBoss_Kurama::FALL);
		return;
	}

	if (m_fAttackTime >= 0.4f && iFrame <= 70)
	{
		dynamic_cast<CBoss_Kurama*>(m_pMonster)->Shoot_FoxSphere(m_iTailedIndex[m_iAttackIndex++ % 9]);
		m_fAttackTime = 0.f;
	}
	else if (iFrame == 85 || iFrame == 86)
	{
		if (!m_isPlayShootSound)
		{
			m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_Kurama_Naruto.wav"), g_fVolume + 0.2f);
			m_isPlayShootSound = true;
		}
		Look_Player();
		dynamic_cast<CBoss_Kurama*>(m_pMonster)->Shoot_Rasengan();
	}

}

void CState_Kurama_SpecialAttack::End_State()
{
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_pMonster->Get_Transform()->Set_NewUp(vUp);

	m_pMonster->Get_RigidBody()->Set_IsGravity(true);
}

void CState_Kurama_SpecialAttack::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt(pTarget->Get_Position());
	}
}

CState_Kurama_SpecialAttack* CState_Kurama_SpecialAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_SpecialAttack* pInstance = new CState_Kurama_SpecialAttack(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_SpecialAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_SpecialAttack::Free()
{
	__super::Free();
}
