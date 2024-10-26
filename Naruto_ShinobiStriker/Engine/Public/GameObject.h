#pragma once

#include "Base.h"
#include "Transform.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_uint			iLevelIndex;
	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	_bool				Get_Dead() { return m_isDead; }
	void				Set_Dead(_bool isDead) { m_isDead = isDead; }

	_bool				IsActive() { return m_isActive; }
	void				IsActive(_bool isActive) { m_isActive = isActive; }

	_uint				Get_LevelIndex() { return m_iLevelIndex; }
	void				Set_LevelIndex(_uint iLevelIndex) { m_iLevelIndex = iLevelIndex; }

	class CTransform*	Get_Transform() {
		if (m_pTransformCom)
			return m_pTransformCom;
		return nullptr;
	}

	_bool				Get_IsGravity() { return m_isGravity; }
	void				Set_IsGravity(_bool isGravity) { m_isGravity = isGravity; }

	_bool				Get_IsCanAttachWall() { return m_isCanAttachWall; }
	void				Set_IsCanAttachWall(_bool isCanAttachWall) { m_isCanAttachWall = isCanAttachWall; }

	_bool				Get_IsAttachWall() { return m_isAttachWall; }
	void				Set_IsAttachWall(_bool isAttachWall) { m_isAttachWall = isAttachWall; }

	_bool				Get_IsWall() { return m_isWall; }
	void				Set_IsWall(_bool isWall) { m_isWall = isWall; }

	_vector				Get_TargetPos() { return XMLoadFloat3(&m_vTargetPos); }
	void				Set_TargetPos(_fvector vTargetPos) { XMStoreFloat3(&m_vTargetPos, vTargetPos); }

	_vector				Get_WallPos() { return XMLoadFloat3(&m_vWallPos); }
	void				Set_WallPos(_fvector vWallPos) { XMStoreFloat3(&m_vWallPos, vWallPos); }

	_vector				Get_Normal() { return XMLoadFloat3(&m_vNormal); }
	void				Set_Normal(_fvector vNormal) { XMStoreFloat3(&m_vNormal, vNormal); }

	_bool				Get_IsCollision() { return m_isCollision; }
	void				Set_IsCollision(_bool isCollision) { m_isCollision = isCollision; }

	_bool				Get_IsGround() { return m_isGround; }
	void				Set_IsGround(_bool isGround) { m_isGround = isGround; }

	_vector				Get_Position() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

	_wstring			Get_LayerTag() { return m_strLayerTag; }
	void				Set_LayerTag(_wstring strLayerTag) { m_strLayerTag = strLayerTag; }

	_bool				Get_IsRender() { return m_isRender; }
	virtual void		Set_IsRender(_bool isRender) { m_isRender = isRender; }

	virtual const _float4x4*		Get_WorldMatrix() { return m_pTransformCom->Get_WorldMatrix_Ptr(); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_LightDepth() { return S_OK; }

public:
	class CComponent* Find_Component(const _wstring& strComponentTag);
	class CComponent* Find_Component(const COMPONENT_TYPE eType);

	_float2 Calculate_ProjPosition(_float vOffsetY);
	_float2 Calculate_ScreenPosition_ClampSide(_float vOffsetY);

	virtual void On_MotionTrail() { m_isRenderMotionTrail = true; };
	virtual void Off_Motiontrail() { m_isRenderMotionTrail = false; }

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	_uint						m_iLevelIndex = {};

	_float3						m_vTargetPos = { 0.f,0.f,0.f };
	_float3						m_vWallPos = { 0.f,0.f,0.f };
	_float3						m_vNormal = { 0.f,0.f,0.f };

	_bool						m_isActive = { true };
	_bool						m_isRender = { true };
	_bool						m_isCloned = { false };
	_bool						m_isDead = { false };
	_bool						m_isGravity = { false };

	_bool						m_isCollision = { false };
	_bool						m_isGround = { false };

	_bool						m_isCanAttachWall = { false };	// 벽에 붙을 수 있는 상황이야?
	_bool						m_isAttachWall = { false };	// 벽에 붙었어?
	_bool						m_isWall = { false };			// 계속 붙어있는 상황이야?
	_bool						m_isRenderMotionTrail = { false };

	_float4x4**					m_BoneMatrices = { nullptr };

	_wstring					m_strLayerTag = {};

protected:
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END