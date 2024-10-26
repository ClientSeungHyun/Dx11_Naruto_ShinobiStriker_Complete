#pragma once

#include "Base.h"

/* 1. 객체들을 그리는 순서에 따라서 보관하고. 정해진 순서대로 객체들의 렌더함수를 호출한다.  */
/* 2. 3D라고 하더라도 객체들을 그리는 순서가 상당히 중요하다. */
/* 3. 하늘, 블렌딩, UI관련된 객체들은 그리는 순서의 관리가 필수적으로 필요하다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_STATICSHADOWOBJ, RG_SHADOWOBJ, RG_NONBLEND, RG_MOTIONTRAIL, RG_NONLIGHT, RG_DISTORTION, RG_EFFECT, RG_BLEND, RG_UI, RG_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	list<class CGameObject*>& Get_RenderList(RENDERGROUP eGroup) { return m_RenderObjects[eGroup]; }
	void Set_IsRenderStaticLight(_bool isRender) { m_isRenderStaticLight = isRender; }
	HRESULT Load_StaticLightDepthTexture(const _wstring& strTextureFilePath);

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	HRESULT Draw();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	list<class CGameObject*>	m_RenderObjects[RG_END];
	class CGameInstance*		m_pGameInstance = { nullptr };

	ID3D11DepthStencilView* m_pLightDepthStencilView = { nullptr };
	ID3D11DepthStencilView* m_pStaticLightDepthStencilView = { nullptr };

	_bool					m_isRenderStaticLight = { false };

private:
	class CShader* m_pShader = { nullptr };
	class CShader* m_pEffectShader = { nullptr };
	class CShader* m_pDistortionShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	_float						m_fDistortionTime = { 0.f };

	_uint						m_iMotionTrailCount = {};
	_uint						m_iMotionTRailMaxCount = { 5 };
	_float4x4					m_MotionTrailWorldMatrix{};

	class CTexture* m_pDistortionTexture = { nullptr };
private:
	HRESULT Render_Priority();
	HRESULT Render_ShadowObj();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_Deferred();
	HRESULT Render_Final();
	HRESULT Render_Blur();
	HRESULT Render_MotionTrail();

	HRESULT Render_NonLights();

	HRESULT Render_Distortion();

	HRESULT Render_Effect();
	HRESULT Render_EffectDown();
	HRESULT Render_EffectBlur();
	HRESULT Render_EffectDeferred();

	HRESULT Render_Blend();
	HRESULT Render_UI();

	HRESULT Render_StaticShadow();

private:
	HRESULT Ready_LightDepthStencilView();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
	_bool	m_isDebugRender = { true };
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END