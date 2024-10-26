#include "stdafx.h"
#include "Skill_Tsukuyomi.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"

#include "Effect_Tsukuyomi01.h"

CSkill_Tsukuyomi::CSkill_Tsukuyomi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_Tsukuyomi::CSkill_Tsukuyomi(const CSkill_Tsukuyomi& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_Tsukuyomi::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Tsukuyomi::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CGameObject::GAMEOBJECT_DESC	Desc{};
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.iLevelIndex = m_iLevelIndex;

	m_pEffect_Tsukuyomi[0] = dynamic_cast<CEffect_Tsukuyomi01*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Tsukuyomi01"), &Desc));
	m_pEffect_Tsukuyomi[1] = dynamic_cast<CEffect_Tsukuyomi01*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Tsukuyomi01"), &Desc));

	m_isActive = false;

	return S_OK;
}

void CSkill_Tsukuyomi::Priority_Update(_float fTimeDelta)
{


}

void CSkill_Tsukuyomi::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	// 정방향
	if (!m_isReverse)
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(m_pTargetMonster);
		if (nullptr != pMonster && pMonster->Get_Fsm()->Get_CurrentState() == CMonster::DIE)
		{
			Safe_Release(m_pTargetMonster);
			m_isActive = false;
		}

		//타겟이 있을 때
		if (m_pTargetMonster != nullptr)
		{
			if (m_fCurrentTime >= 0.2f && !m_isSummonSecond)
			{
				if (m_isBoss)
					m_pEffect_Tsukuyomi[1]->Appear(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.1f, 6.f, CEffect_Tsukuyomi01::ACTION);
				else
					m_pEffect_Tsukuyomi[1]->Appear(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.1f, 3.f, CEffect_Tsukuyomi01::ACTION);

				m_vEffectColor = _float4(0.9f, 0.f, 0.f, 0.5f);
				m_isSummonSecond = true;
			}

			if (m_isSummonSecond)
			{
				if (m_fCurrentTime <= 0.6f)
				{
					m_fCurrentScale += 5.f * fTimeDelta;
					if (m_fCurrentScale >= 2.9f)
					{
						m_fCurrentScale = 2.9f;
					}
				}
				else 
				{
					m_fCurrentScale -= 5.f * fTimeDelta;
					if (m_fCurrentScale <= 0.1f)
					{
						m_fCurrentScale = 0.1f;
						m_fCurrentTime = 0.f;
						m_isReverse = true;
					}
				}
			}
			else
			{
				m_fCurrentScale -= 5.f * fTimeDelta;
				if (m_fCurrentScale <= 0.1f)
				{
					m_fCurrentScale = 0.1f;
				}
			}
		}
		// 타겟이 없을 때
		else
		{
			if (m_fCurrentTime >= 0.2f && !m_isSummonSecond)
			{
				m_pEffect_Tsukuyomi[1]->Appear(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.1f, 3.f, CEffect_Tsukuyomi01::ACTION);

				m_isSummonSecond = true;
			}
			else if (m_fCurrentTime >= 2.5f)
			{
				m_pEffect_Tsukuyomi[0]->DisAppear();
				m_pEffect_Tsukuyomi[1]->DisAppear();

				Safe_Release(m_pTargetMonster);
				m_isActive = false;
			}

			if (m_isSummonSecond)
			{
				m_fCurrentScale -= 5.f * fTimeDelta;
				if (m_fCurrentScale <= 0.1f)
				{
					m_fCurrentScale = 0.1f;
				}
			}
		}
	}

	// 반대 방향
	if (m_isReverse)
	{
		if (m_fCurrentTime >= 1.2f && !m_isReverseFirst)
		{
			dynamic_cast<CMonster*>(m_pTargetMonster)->Change_State_HitDown();
			dynamic_cast<CMonster*>(m_pTargetMonster)->Damaged(20.f);

			m_pEffect_Tsukuyomi[1]->Start_Reverse();
			m_vEffectColor = _float4(1.2f, 0.f, 0.f, 0.5f);
			m_fCurrentScale = 2.9f;
			m_isReverseFirst = true;
		}
		else if (m_fCurrentTime >= 1.4f && !m_isReverseSecond)
		{
			m_pEffect_Tsukuyomi[0]->Start_Reverse();
			m_isReverseSecond = true;
		}

		if (m_isReverseFirst)
		{
			m_fCurrentScale -= 5.f * fTimeDelta;
			m_vEffectColor.x -= 3.f * fTimeDelta;
			if (m_fCurrentScale <= 0.1f)
			{
				m_fCurrentScale = 0.1f;
			}
		}
	}

	if (!m_pEffect_Tsukuyomi[0]->IsActive() && !m_pEffect_Tsukuyomi[1]->IsActive())
	{
		Safe_Release(m_pTargetMonster);
		m_isActive = false;
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->BillBoard();

}

void CSkill_Tsukuyomi::Late_Update(_float fTimeDelta)
{
	m_pSoundCom->Update(fTimeDelta);

	if(m_fCurrentScale > 0.1f)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CSkill_Tsukuyomi::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSkill_Tsukuyomi::Appear(CGameObject* pTargetMonster, _float fScale)
{
	if (m_isActive)
		return;

	_vector vPos;
	if (pTargetMonster != nullptr)
	{
		m_pTargetMonster = pTargetMonster;
		Safe_AddRef(m_pTargetMonster);
		
		if (dynamic_cast<CMonster*>(pTargetMonster)->Get_MonsterID() == KURAMA
			|| dynamic_cast<CMonster*>(pTargetMonster)->Get_MonsterID() == SUSANOO)
		{
			_vector vLook = XMVector3Normalize(m_pTargetMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK));
			vPos = pTargetMonster->Get_Position() + XMVectorSet(0.f, 4.f, 0.f, 0.f) ;
			m_isBoss = true;
		}
		else
		{
			_vector vLook = XMVector3Normalize(m_pTargetMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK));
			vPos = pTargetMonster->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
			m_isBoss = false;
		}

		dynamic_cast<CMonster*>(pTargetMonster)->Change_State_Groggy();
	}
	else
	{
		_vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));
		vPos = m_pOwner->Get_Position() + vLook * 10.f + XMVectorSet(0.f, 1.f, 0.f, 0.f);
		m_isBoss = false;
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fCurrentTime = 0.f;
	m_fCurrentScale = 2.5f;
	m_vEffectColor = _float4(0.f, 0.f, 0.9f, 0.5f);

	m_isSummonSecond = false;
	m_isReverse = false;
	m_isReverseFirst = false;
	m_isReverseSecond = false;
	m_isActive = true;

	if (m_isBoss)
		m_pEffect_Tsukuyomi[0]->Appear(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.1f, 6.f, CEffect_Tsukuyomi01::BASE);
	else
		m_pEffect_Tsukuyomi[0]->Appear(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.1f, 3.f, CEffect_Tsukuyomi01::BASE);


	m_pSoundCom->Play3D(TEXT("Sarin.wav"), g_fVolume + 0.2f);
}

void CSkill_Tsukuyomi::DisAppear()
{
	m_pSoundCom->Stop();
	m_pTargetMonster = nullptr;
	m_isActive = false;
}

HRESULT CSkill_Tsukuyomi::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CSkill_Tsukuyomi* CSkill_Tsukuyomi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Tsukuyomi* pInstance = new CSkill_Tsukuyomi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_Tsukuyomi"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_Tsukuyomi::Clone(void* pArg)
{
	CSkill_Tsukuyomi* pInstance = new CSkill_Tsukuyomi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_Tsukuyomi"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Tsukuyomi::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_AddRef(m_pTargetMonster);
}
