#pragma once

#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSkill_Tsukuyomi : public CSkill
{

protected:
	CSkill_Tsukuyomi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Tsukuyomi(const CSkill_Tsukuyomi& Prototype);
	virtual ~CSkill_Tsukuyomi() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(class CGameObject* pTargetMonster = nullptr, _float fScale = 3.f);
	void		DisAppear();

protected:
	class CShader*			m_pShaderCom = { nullptr };
	class CTexture*			m_pTextureCom = { nullptr };
	class CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };

	class CGameObject*			m_pTargetMonster = { nullptr };

	_float						m_fCurrentScale = {};
	_float4						m_vEffectColor = {};

	_bool						m_isReverse = { false };
	_bool						m_isSummonSecond = { false };
	_bool						m_isReverseFirst = { false };
	_bool						m_isReverseSecond = { false };
	_bool						m_isBoss = { false };

	class CEffect_Tsukuyomi01*	m_pEffect_Tsukuyomi[2] = {nullptr, nullptr};

private:
	HRESULT Ready_Components();

public:
	static CSkill_Tsukuyomi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END