#pragma once

/* �����̳� ������Ʈ�ȿ� �߰��� �� �ִ� �ϳ��� ��ǰ��ü���� �θ� Ŭ���� */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentWorldMatrix = { nullptr };
		_int iPartID;
	}PARTOBJ_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	class CModel*	Get_Model() { return m_pModelCom; }

	virtual const _float4x4* Get_WorldMatrix() { return &m_WorldMatrix; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	/* m_pTransformCom->m_WorldMatrix * �θ��� ���� */
	class CModel* m_pModelCom = { nullptr };

	const _float4x4*		m_pParentMatrix = { nullptr };
	_float4x4				m_WorldMatrix = {};

	_int					m_iCurretLevelID = {};

protected:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pContantName);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END