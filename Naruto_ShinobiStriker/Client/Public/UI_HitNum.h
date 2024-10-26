#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_HitNum final : public CUIObject
{
public:
	enum HITUI_TYPE { HITNUM, BIGTEXT, SMALLTEXT, HITUI_END };

public:
	typedef struct : public CUIObject::UI_DESC
	{
		_float* pPlayerHP;
		class CPlayer* pPlayer;
	}HPUI_DESC;

private:
	CUI_HitNum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_HitNum(const CUI_HitNum& Prototype);
	virtual ~CUI_HitNum() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Appear(_float fTimeDelta) override;
	virtual void Disappear(_float fTimeDelta) override;

	virtual void Start_Appear() override;
	virtual void Start_Disappear() override;

	void Add_HitCount();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };	//ผýภฺต้
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	CTexture*		m_pHitTextTexture = { nullptr };

	_uint			m_iHitCount = { 0 };

	_float			m_fScale = { 0.f };
	_float			m_fLifeTime = { 0.f };

	_float3			m_vOffset = {};

	list<_uint>		m_HitNumList;

	_matrix			HitNumMatrix[HITUI_END];

private:
	HRESULT Ready_Components();

	void	Convert_NumToStr();

public:
	static CUI_HitNum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END